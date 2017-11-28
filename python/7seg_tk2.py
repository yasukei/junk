#!/usr/bin/env python
# -*- config: utf-8 -*-

# refer: http://docs.python.jp/2/library/tkinter.html

class SsdState:
    def __init__(self):
        pass
    def onEntry(self):
        pass
    def onExit(self):
        pass
    def onTick(self, elapsed_timeMillisec):
        pass

class SsdFrame:
    def __init__(self, picture, duration_time):
        self.picture = picture
        self.duration_time = duration_time

class SsdAnimation:
    def __init__(self):
        pass
    def restartAnimation(self):
        pass
    def getPicture(self, elapsed_timeMillisec):
        pass

class SsdStaticAnimation:
    def __init__(self, frames):
        self.frames = frames
        self.restartAnimation()

    def restartAnimation(self):
        self.elapsed_timeMillisec = 0

    def getPicture(self, elapsed_timeMillisec):
        self.elapsed_timeMillisec += elapsed_timeMillisec
        for f in self.frames:
            if f.duration_time > self.elapsed_timeMillisec:
                return f.picture
        self.elapsed_timeMillisec = 0
        last_index = len(self.frames)
        return self.frames.index(last_index).picture

class SsdDynamicAnimation:
    def __init__(self, getDynamicPicture):
        self.getDynamicPicture = getDynamicPicture
        self.restartAnimation()

    def restartAnimation(self):
        self.elapsed_timeMillisec = 0

    def getPicture(self, elapsed_timeMillisec):
        self.elapsed_timeMillisec += elapsed_timeMillisec
        return self.getDynamicPicture(self.elapsed_timeMillisec)

class SsdTestMode(SsdState):
    def __init__(self, ssdDriver):
        self.animation = SsdStaticAnimation([
            SsdFrame(SsdDriver.SEG_F,    250),
            SsdFrame(SsdDriver.SEG_A,    250),
            SsdFrame(SsdDriver.SEG_B,    250),
            SsdFrame(SsdDriver.SEG_C,    250),
            SsdFrame(SsdDriver.SEG_D,    250),
            SsdFrame(SsdDriver.SEG_E,    250),
            SsdFrame(SsdDriver.SEG_G,    250),
            SsdFrame(SsdDriver.SEG_DP,   250),
            SsdFrame(SsdDriver.SEG_NONE, 250),
            ])
        self.ssdDriver = ssdDriver

    def onTick(self, elapsed_timeMillisec):
        p = self.animation.getPicture(elapsed_timeMillisec)
        self.ssdDriver.display(p, p)

class SsdNormalMode(SsdState):
    def __init__(self, ssdDriver, stateManager, configManager, loggingManager):
        self.animationForDebugForLeft = SsdStaticAnimation([
            SsdFrame(SsdDriver.CHAR_HIPHEN, 0),
            ])
        self.animationForDebugForRight = SsdStaticAnimation([
            SsdFrame(SsdDriver.CHAR_HIPHEN, 0),
            ])
        self.animationForProgramForLeft = SsdStaticAnimation([
            SsdFrame(SsdDriver.CHAR_HIPHEN, 500),
            SsdFrame(SsdDriver.SEG_NONE,    500),
            ])
        self.animationForProgramForRight = SsdStaticAnimation([
            SsdFrame(SsdDriver.CHAR_HIPHEN, 500),
            SsdFrame(SsdDriver.SEG_NONE,    500),
            ])
        self.sign2 = None
        self.sign3 = None
        self.animationForRunForLeft = SsdDynamicAnimation(
            lambda elapsed_timeMillisec: self.sign2 if self.sign2 != None else self.ssdDriver.CHAR_HIPHEN
            )
        self.animationForRunForRight = SsdDynamicAnimation(
            lambda elapsed_timeMillisec: self.sign3 if self.sign3 != None else self.ssdDriver.CHAR_HIPHEN
            )
        self.changeCurrentAnimation(self.animationForDebugForLeft, self.animationForDebugForRight)
        self.ssdDriver = ssdDriver
        self.stateManager.subscribe(self.notifyStateManagerUpdated)
        self.configManager.subscribe(self.notifyConfigManagerUpdated)
        self.loggingManager.subscribe(self.notifyLoggingManagerUpdated)

    def changeCurrentAnimation(self, left_anim, right_anim):
        self.current_animationForLeft = left_anim
        self.current_animationForRight = right_anim
        self.current_animationForLeft.restartAnimation()
        self.current_animationForRight.restartAnimation()

    def notifyStateManagerUpdated(self):
        s = self.stateManager.getState()
        if s == self.stateManager.RUN:
            self.changeCurrentAnimation(self.animationForRunForLeft, self.animationForRunForRight)
        elif s == self.stateManager.DEBUG:
            self.changeCurrentAnimation(self.animationForDebugForLeft, self.animationForDebugForRight)
        else:
            self.changeCurrentAnimation(self.animationForProgramForLeft, self.animationForProgramForRight)

    def notifyLoggingManagerUpdated(self):
        pass # TODO: update myself for displaying dot

    def notifyConfigManagerUpdated(self):
        self.sign2 = self.configManager.getSign2()
        self.sign3 = self.configManager.getSign3()

    def onTick(self, elapsed_timeMillisec):
        l = self.current_animationForLeft.getPicture(elapsed_timeMillisec)
        r = self.current_animationForRight.getPicture(elapsed_timeMillisec)
        self.ssdDriver.display(l, r)

class SsdFaultMode:
    pass

class SsdServiceSwitchMode:
    pass

class SsdRestoreMode:
    pass

class SsdCriticalFaultMode:
    pass

class SsdTransition:
    def __init__(self, src_state, dst_state, event):
        self.src_state = src_state
        self.dst_state = dst_state
        self.event = event

class SsdStateMachine:
    def __init__(self):
        self.state = None
        self.transitions = []
    def setInitialState(self, state):
        self.state = state
    def addTransition(self, src_state, dst_state, event):
        transition = SsdTransition(src_state, dst_state, event)
        self.transitions.append(transition)
    def onEvent(self, event):
        for t in self.transitions:
            if self.state == t.src_state and event == t.event:
                self.transit(t.dst_state)
    def transit(self, next_state):
        self.state.onExit()
        next_state.onEntry()
        self.state = next_state

class SsdManager:
    eEvent_onInitilized                        = 1
    eEvent_onRestore                           = 2
    eEvent_onCriticalFault                     = 3
    eEvent_onFaultOccured                      = 4
    eEvent_onFaultRemoved                      = 5
    eEvent_onServiceSwitchOn                   = 6
    eEvent_onServiceSwitchServiceFinished      = 7
    eEvent_xxx                                 = 8

    def __init__(self, stateManager, configManager, eventManager, loggingManager, serviceSwitchDriver):
        testMode           = SsdTestMode()
        normalMode         = SsdNormalMode(stateManager, configManager, loggingManager)
        faultMode          = SsdFaultMode(eventManager)
        serviceSwitchMode  = SsdServiceSwitchMode(serviceSwitchDriver, configManager, loggingManager)
        restoreMode        = SsdRestoreMode(configManager)
        criticalFaultMode  = SsdCriticalFaultMode()

        ssdSM = SsdStateMachine()
        ssdSM.setInitialState(testMode)
        ssdSM.addTransition(testMode,           normalMode,        eEvent_onInitilized)
        ssdSM.addTransition(testMode,           restoreMode,       eEvent_onRestore)
        ssdSM.addTransition(testMode,           criticalFaultMode, eEvent_onCriticalFault)
        ssdSM.addTransition(normalMode,         faultMode,         eEvent_onFaultOccured)
        ssdSM.addTransition(normalMode,         serviceSwitchMode, eEvent_onServiceSwitchOn)
        ssdSM.addTransition(normalMode,         criticalFaultMode, eEvent_onCriticalFault)
        ssdSM.addTransition(faultMode,          serviceSwitchMode, eEvent_onServiceSwitchOn)
        ssdSM.addTransition(faultMode,          normalMode,        eEvent_onFaultRemoved)
        ssdSM.addTransition(faultMode,          criticalFaultMode, eEvent_onCriticalFault)
        ssdSM.addTransition(serviceSwitchMode,  normalMode,        eEvent_onServiceSwitchServiceFinished)
        ssdSM.addTransition(serviceSwitchMode,  criticalFaultMode, eEvent_onCriticalFault)
        ssdSM.addTransition(restoreMode,        faultMode,         eEvent_onFaultOccured)
        ssdSM.addTransition(restoreMode,        criticalFaultMode, eEvent_onCriticalFault)
        #ssdStateMachine.addTransition(serviceSwitchMode,  faultMode,         eEvent_onServiceSwitchServiceFinished)
        self.ssdSM = ssdSM

        serviceSwitchDriver.bindSwitchOn(0, lambda : self.onEvent(eEvent_onServiceSwitchOn))
        serviceSwitchDriver.bindSwitchOff(lambda: self.onEvent(eEvent_onServiceSwitchServiceFinished))
        self.serviceSwitchDriver = serviceSwitchDriver

    def onEvent(event):
        self.ssdSM.onEvent(event)

print "hello2!"

