#!/usr/bin/env python
# -*- config: utf-8 -*-

# refer: http://docs.python.jp/2/library/tkinter.html

class Publisher:
    def __init__(self):
        self.subscribers = []

    def subscribe(self, callback):
        self.subscribers.append(callback)

    def update(self):
        for s in self.subscribers:
            s()

class StateManager:
    def __init__(self):
        pass

class ConfigManager:
    def __init__(self):
        pass

class EventManager:
    def __init__(self):
        pass

class LoggingManager:
    NO_SETTING              = 1
    WAITING_FOR_TRIGGER     = 1
    TRIGGERED_AND_LOGGING   = 1
    COMPLETED               = 1

    def __init__(self):
        pass
    def getState(self):
        # TODO
        return self.NO_SETTING

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
    def __init__(self):
        pass
    def getPicture(self):
        pass
    def getDurationTime(self):
        pass

class SsdStaticFrame(SsdFrame):
    def __init__(self, picture, duration_time):
        SsdFrame.__init__(self)
        self.picture = picture
        self.duration_time = duration_time

    def getPicture(self):
        return self.picture

    def getDurationTime(self):
        return self.duration_time

class SsdDynamicFrame(SsdFrame):
    def __init__(self, getPicture, duration_time):
        SsdFrame.__init__(self)
        self.getPicture = getPicture
        self.duration_time = duration_time

    def getPicture(self):
        return self.getPicture()

    def getDurationTime(self):
        return self.duration_time

class SsdAnimation:
    def __init__(self, frames):
        self.frames = frames
        self.restartAnimation()

    def restartAnimation(self):
        self.elapsed_timeMillisec = 0

    def getPicture(self, elapsed_timeMillisec):
        self.elapsed_timeMillisec += elapsed_timeMillisec
        for f in self.frames:
            if f.getDurationTime() > self.elapsed_timeMillisec:
                return f.getPicture()
        self.elapsed_timeMillisec = 0
        last_index = len(self.frames)
        return self.frames.index(last_index).getPicture()

class SsdTestMode(SsdState):
    def __init__(self, ssdDriver):
        self.animation = SsdAnimation([
            SsdStaticFrame(SsdDriver.SEG_F,    250),
            SsdStaticFrame(SsdDriver.SEG_A,    250),
            SsdStaticFrame(SsdDriver.SEG_B,    250),
            SsdStaticFrame(SsdDriver.SEG_C,    250),
            SsdStaticFrame(SsdDriver.SEG_D,    250),
            SsdStaticFrame(SsdDriver.SEG_E,    250),
            SsdStaticFrame(SsdDriver.SEG_G,    250),
            SsdStaticFrame(SsdDriver.SEG_DP,   250),
            SsdStaticFrame(SsdDriver.SEG_NONE, 250),
            ])
        self.ssdDriver = ssdDriver

    def onTick(self, elapsed_timeMillisec):
        p = self.animation.getPicture(elapsed_timeMillisec)
        self.ssdDriver.display(p, p)

class SsdNormalMode(SsdState):
    def __init__(self, ssdDriver, stateManager, configManager, loggingManager):
        self.animationForDebugForLeft = SsdAnimation([
            SsdStaticFrame(SsdDriver.CHAR_HIPHEN, 0),
            ])
        self.animationForDebugForRight = SsdAnimation([
            SsdStaticFrame(SsdDriver.CHAR_HIPHEN, 0),
            ])
        self.animationForProgramForLeft = SsdAnimation([
            SsdStaticFrame(SsdDriver.CHAR_HIPHEN, 500),
            SsdStaticFrame(SsdDriver.SEG_NONE,    500),
            ])
        self.animationForProgramForRight = SsdAnimation([
            SsdStaticFrame(SsdDriver.CHAR_HIPHEN, 500),
            SsdStaticFrame(SsdDriver.SEG_NONE,    500),
            ])
        self.sign2 = None
        self.sign3 = None
        self.animationForRunForLeft = SsdAnimation([
            SsdDynamicFrame(
                lambda elapsed_timeMillisec: self.sign2 if self.sign2 != None else self.ssdDriver.CHAR_HIPHEN,
                0)
            ])
        self.animationForRunForRight = SsdAnimation([
            SsdDynamicFrame(
                lambda elapsed_timeMillisec: self.sign3 if self.sign3 != None else self.ssdDriver.CHAR_HIPHEN,
                0)
            )
        self.changeCurrentAnimation(self.animationForDebugForLeft, self.animationForDebugForRight)
        self.dotOffAnimation = SsdAnimation([
            SsdStaticFrame(SsdDriver.SEG_NONE,  0),
            ])
        self.dotOnAnimation = SsdAnimation([
            SsdStaticFrame(SsdDriver.SEG_DP,    0),
            ])
        self.dotBlinkAnimation = SsdAnimation([
            SsdStaticFrame(SsdDriver.SEG_NONE,  500),
            SsdStaticFrame(SsdDriver.SEG_DP,    500),
            ])
        self.changeCurrentDotAnimation(self.dotOffAnimation, self.dotOffAnimation)
        self.ssdDriver = ssdDriver
        self.stateManager.subscribe(self.notifyStateManagerUpdated)
        self.configManager.subscribe(self.notifyConfigManagerUpdated)
        self.loggingManager.subscribe(self.notifyLoggingManagerUpdated)

    def changeCurrentAnimation(self, left_anim, right_anim):
        self.current_animationForLeft = left_anim
        self.current_animationForRight = right_anim
        self.current_animationForLeft.restartAnimation()
        self.current_animationForRight.restartAnimation()

    def changeCurrentDotAnimation(self, left_anim, right_anim):
        self.current_dotAnimationForLeft = left_anim
        self.current_dotAnimationForRight = right_anim
        self.current_dotAnimationForLeft.restartAnimation()
        self.current_dotAnimationForRight.restartAnimation()

    def notifyStateManagerUpdated(self):
        s = self.stateManager.getState()
        if s == self.stateManager.RUN:
            self.changeCurrentAnimation(self.animationForRunForLeft, self.animationForRunForRight)
        elif s == self.stateManager.DEBUG:
            self.changeCurrentAnimation(self.animationForDebugForLeft, self.animationForDebugForRight)
        else:
            self.changeCurrentAnimation(self.animationForProgramForLeft, self.animationForProgramForRight)

    def notifyConfigManagerUpdated(self):
        self.sign2 = self.configManager.getSign2()
        self.sign3 = self.configManager.getSign3()

    def notifyLoggingManagerUpdated(self):
        s = self.loggingManager.getState()
        if s == self.loggingManager.COMPLETED:
            self.changeCurrentDotAnimation(self.dotOffAnimation, self.dotOnAnimation)
        elif s == self.loggingManager.TRIGGERED_AND_LOGGING:
            self.changeCurrentDotAnimation(self.dotOnAnimation, self.dotBlinkAnimation)
        elif s == self.loggingManager.WAITING_FOR_TRIGGER:
            self.changeCurrentDotAnimation(self.dotOnAnimation, self.dotOffAnimation)
        else:
            self.changeCurrentDotAnimation(self.dotOffAnimation, self.dotOffAnimation)

    def onTick(self, elapsed_timeMillisec):
        l = self.current_animationForLeft.getPicture(elapsed_timeMillisec)
        r = self.current_animationForRight.getPicture(elapsed_timeMillisec)
        ld = self.current_dotAnimationForLeft.getPicture(elapsed_timeMillisec)
        rd = self.current_dotAnimationForRight.getPicture(elapsed_timeMillisec)
        self.ssdDriver.display(l + ld, r + rd)

class FaultRecord:
    def __init__(self, errorCode, annexs=[]):
        self.errorCode = errorCode
        self.annexs = annexs

class FaultLogTranslationKnowledge:
    def __init__(self, eventId, translationMethod):
        self.eventId = eventId
        self.translationMethod = translationMethod

    def translate(self, faultLog):
        return self.translationMethod(faultLog)

class FaultLogTranslator:
    def __init__(self):
        self.transKnowledges = [
                    FaultLogTranslationKnowledge(0x1000, self.translationFor0x1000),
                    FaultLogTranslationKnowledge(0x1001, self.translationFor0x1001),
                ]

    def translate(self, faultLogs=[]):
        records = []
        for f in faultLogs:
            record = self.translateToFaultRecord(f)
        return records

    def translateToFaultRecord(self, faultLog):
        eventId = faultLog.getEventId()
        for k in self.transKnowledges:
            if k.eventId == eventId:
                k.translate(faultLog)
        return None

    def translationFor0x1000(self, faultLog):
        return FaultRecord(0x10, [])

    def translationFor0x1001(self, faultLog):
        return FaultRecord(0x10, [0x01])

class FaultWatcher(Publisher):
    def __init__(self, eventManager):
        self.Publisher.__init__(self)
        self.faultLog = []
        self.updateCount = eventManager.getUpdateCount()
        self.eventManager = eventManager

    def cyclicTask(self):
        if self.eventManager.getUpdateCount() != self.updateCount:
            faultLogs = self.eventManager.readFaultLogs()
            self.lock()
            self.faultRecords = self.translator.translate(faultLogs)
            self.unlock()
            self.update()

    def readFaultRecords(self):
        self.lock()
        r = self.faultRecords
        self.unlock()
        return r

    def lock(self):
        pass
    def unlock(self):
        pass

class SsdFaultMode:
    def __init__(self, ssdDriver, faultWatcher, ssdStateMachine):
        self.ssdSM = ssdStateMachine
        self.faultRecordsUpdated = False
        self.faultWatcher.subscribe(self.notifyFaultWatcherUpdated)
        self.animation = None

    def notifyFaultWatcherUpdated(self):
        self.lock()
        faultRecords = self.faultWatcher.readFaultRecords()
        self.faultRecordsUpdated = True
        self.unlock()
        self.updateAnimation(faultRecords)
        if len(faultRecords) == 0:
            self.ssdSM.onEvent(eEvent_onFaultRemoved)
        else:
            self.ssdSM.onEvent(eEvent_onFaultOccured)

    def updateAnimation(self, faultRecords):
        self.animation = []
        for r in faultRecords:
            # need to divide errorCode into left and right animation separately
            self.animation.append(
                    SsdStaticFrame(self.convertHexToSsdCode(r.errorCode)),
                    1000
                    )
            self.animation.append(SsdDriver.SEG_NONE, 100)
            for annex in r.annexs:
                self.animation.append(
                        SsdStaticFrame(self.convertHexToSsdCode(annex))
                        )
                self.animation.append(SsdDriver.SEG_NONE, 300)

    def convertHexToSsdCode(byteValue):
        pass

    def onTick(self, elapsed_timeMillisec):
        l = self.animation.getPicture() # need to adjust this with self.updateAnimation()
        r = self.animation.getPicture() # need to adjust this with self.updateAnimation()
        self.ssdDriver.display(l, r)

    def lock(self):
        pass
    def unlock(self):
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

