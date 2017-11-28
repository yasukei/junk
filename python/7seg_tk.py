#!/usr/bin/env python
# -*- config: utf-8 -*-

# refer: http://docs.python.jp/2/library/tkinter.html

from Tkinter import *
import collections

class Publisher:
    def __init__(self):
        self.subscribers = []

    def subscribe(self, callback):
        self.subscribers.append(callback)

    def update(self):
        for s in self.subscribers:
            s()

class StateManager(Publisher):
    INITIALIZING = 'initializing'
    PROGRAM = 'program'
    DEBUG = 'debug'
    RUN = 'run'

    def __init__(self, stateSelector, timer):
        Publisher.__init__(self)
        self.stateSelector = stateSelector
        self.state = StateManager.INITIALIZING
        self.timer = timer
        self.tickTimeMillisec = 100
        self.updateState()

    def updateState(self):
        s = self.stateSelector.getState()
        if s == self.stateSelector.INITIALIZING:
            n = self.INITIALIZING
        elif s == self.stateSelector.PROGRAM:
            n = self.PROGRAM
        elif s == self.stateSelector.DEBUG:
            n = self.DEBUG
        else:
            n = self.RUN
        self.transit(n)
        self.timer.after(self.tickTimeMillisec, self.updateState)

    def transit(self, nextState):
        if nextState != self.state:
            self.state = nextState
            self.update()

    def getState(self):
        return self.state

class EventManager(Publisher):
    def __init__(self):
        Publisher.__init__(self)

    def hasCriticalFault(self):
        # TODO
        return False

    CRITICAL_FAULT = 'critical_fault'

class ConfigManager(Publisher):
    def __init__(self):
        Publisher.__init__(self)
        self.signature = 'ABCD'
        #self.signature = None

    def setSignature(self, signature):
        self.signature = signature

    def getSignature(self):
        return self.signature

class LoggingManager(Publisher):
    NO_LOGGING_SETTINGS = '1'
    WAITING_FOR_TRIGGER = '2'
    TRIGGERRED = '3'
    COMPLETED = '4'

    def __init__(self, stateSelector, timer):
        Publisher.__init__(self)
        self.stateSelector = stateSelector
        self.state = LoggingManager.NO_LOGGING_SETTINGS
        self.timer = timer
        self.tickTimeMillisec = 100
        self.updateState()

    def updateState(self):
        s = self.stateSelector.getState()
        if s == self.stateSelector.NO_LOGGING_SETTINGS:
            n = self.NO_LOGGING_SETTINGS
        elif s == self.stateSelector.WAITING_FOR_TRIGGER:
            n = self.WAITING_FOR_TRIGGER
        elif s == self.stateSelector.TRIGGERRED:
            n = self.TRIGGERRED
        else:
            n = self.COMPLETED
        self.transit(n)
        self.timer.after(self.tickTimeMillisec, self.updateState)

    def transit(self, nextState):
        if nextState != self.state:
            self.state = nextState
            self.update()

    def getState(self):
        return self.state

class SsdCanvas(Canvas):
    """
    https://en.wikipedia.org/wiki/Seven-segment_display
    """
    def configure(self):
        ox = 5
        oy = 5
        length = 50
        width = 10
        self.on_color = 'yellow'
        self.off_color = 'gray'
        self.tc = self.create_rectangle(
                    ox + width,
                    oy,
                    ox + width + length,
                    oy + width,
                    fill=self.off_color,
                    tag=SsdCanvas.SEG_A
                    )
        self.tr = self.create_rectangle(
                    ox + width + length,
                    oy + width,
                    ox + width + length + width,
                    oy + width + length,
                    fill=self.off_color,
                    tag=SsdCanvas.SEG_B
                    )
        self.br = self.create_rectangle(
                    ox + width + length,
                    oy + width + length + width,
                    ox + width + length + width,
                    oy + width + length + width + length,
                    fill=self.off_color,
                    tag=SsdCanvas.SEG_C
                    )
        self.bc = self.create_rectangle(
                    ox + width,
                    oy + width + length + width + length,
                    ox + width + length,
                    oy + width + length + width + length + width,
                    fill=self.off_color,
                    tag=SsdCanvas.SEG_D
                    )
        self.bl = self.create_rectangle(
                    ox,
                    oy + width + length + width,
                    ox + width,
                    oy + width + length + width + length,
                    fill=self.off_color,
                    tag=SsdCanvas.SEG_E
                    )
        self.tl = self.create_rectangle(
                    ox,
                    oy + width,
                    ox + width,
                    oy + width + length,
                    fill=self.off_color,
                    tag=SsdCanvas.SEG_F
                    )
        self.mc = self.create_rectangle(
                    ox + width,
                    oy + width + length,
                    ox + width + length,
                    oy + width + length + width,
                    fill=self.off_color,
                    tag=SsdCanvas.SEG_G
                    )
        self.dot = self.create_rectangle(
                    ox + width + length + width,
                    oy + width + length + width + length,
                    ox + width + length + width + width,
                    oy + width + length + width + length + width,
                    fill=self.off_color,
                    tag=SsdCanvas.SEG_DP
                    )
        self.pack()

    def __init__(self, master=None):
        Canvas.__init__(self, master)
        self.pack()
        self.configure()

    def on(self, segment_name):
        i = self.find_withtag(segment_name)
        self.itemconfig(i, fill=self.on_color)

    def off(self, segment_name):
        i = self.find_withtag(segment_name)
        self.itemconfig(i, fill=self.off_color)

    def display(self, display_segments=''):
        for s in self.segments:
            if s in display_segments:
                self.on(s)
            else:
                self.off(s)

    SEG_A = 'A'
    SEG_B = 'B'
    SEG_C = 'C'
    SEG_D = 'D'
    SEG_E = 'E'
    SEG_F = 'F'
    SEG_G = 'G'
    SEG_DP ='H'
    segments = [SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_DP]

class SsdDriver:
    SEG_A  = SsdCanvas.SEG_A
    SEG_B  = SsdCanvas.SEG_B
    SEG_C  = SsdCanvas.SEG_C
    SEG_D  = SsdCanvas.SEG_D
    SEG_E  = SsdCanvas.SEG_E
    SEG_F  = SsdCanvas.SEG_F
    SEG_G  = SsdCanvas.SEG_G
    SEG_DP = SsdCanvas.SEG_DP
    CHAR_0 = '0'
    CHAR_1 = '1'
    CHAR_2 = '2'
    CHAR_3 = '3'
    CHAR_4 = '4'
    CHAR_5 = '5'
    CHAR_6 = '6'
    CHAR_7 = '7'
    CHAR_8 = '8'
    CHAR_9 = '9'
    CHAR_A = 'A'
    CHAR_B = 'B'
    CHAR_C = 'C'
    CHAR_D = 'D'
    CHAR_E = 'E'
    CHAR_F = 'F'
    CHAR_G = 'G'
    CHAR_H = 'H'
    CHAR_I = 'I'
    CHAR_J = 'J'
    CHAR_K = 'K'
    CHAR_L = 'L'
    CHAR_M = 'M'
    CHAR_N = 'N'
    CHAR_O = 'O'
    CHAR_P = 'P'
    CHAR_Q = 'Q'
    CHAR_R = 'R'
    CHAR_S = 'S'
    CHAR_T = 'T'
    CHAR_U = 'U'
    CHAR_V = 'V'
    CHAR_W = 'W'
    CHAR_X = 'X'
    CHAR_Y = 'Y'
    CHAR_Z = 'Z'
    CHAR_HYPHEN = '-'
    CHAR_NONE = ''

    def __init__(self, hardware1, hardware2):
        SEG_A   = hardware1.SEG_A 
        SEG_B   = hardware1.SEG_B 
        SEG_C   = hardware1.SEG_C 
        SEG_D   = hardware1.SEG_D 
        SEG_E   = hardware1.SEG_E 
        SEG_F   = hardware1.SEG_F 
        SEG_G   = hardware1.SEG_G 
        SEG_DP  = hardware1.SEG_DP
        xxxxx   = ''
        self.dic = {
                self.CHAR_0         : SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F + xxxxx,
                self.CHAR_1         : xxxxx + SEG_B + SEG_C + xxxxx + xxxxx + xxxxx + xxxxx,
                self.CHAR_2         : SEG_A + SEG_B + xxxxx + SEG_D + SEG_E + xxxxx + SEG_G,
                self.CHAR_3         : SEG_A + SEG_B + SEG_C + SEG_D + xxxxx + xxxxx + SEG_G,
                self.CHAR_4         : xxxxx + SEG_B + SEG_C + xxxxx + xxxxx + SEG_F + SEG_G,
                self.CHAR_5         : SEG_A + xxxxx + SEG_C + SEG_D + xxxxx + SEG_F + SEG_G,
                self.CHAR_6         : SEG_A + xxxxx + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G,
                self.CHAR_7         : SEG_A + SEG_B + SEG_C + xxxxx + xxxxx + xxxxx + xxxxx,
                self.CHAR_8         : SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G,
                self.CHAR_9         : SEG_A + SEG_B + SEG_C + SEG_D + xxxxx + SEG_F + SEG_G,
                self.CHAR_A         : SEG_A + SEG_B + SEG_C + xxxxx + SEG_E + SEG_F + SEG_G,
                self.CHAR_B         : xxxxx + xxxxx + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G,
                self.CHAR_C         : SEG_A + xxxxx + xxxxx + SEG_D + SEG_E + SEG_F + xxxxx,
                self.CHAR_D         : xxxxx + SEG_B + SEG_C + SEG_D + SEG_E + xxxxx + SEG_G,
                self.CHAR_E         : SEG_A + xxxxx + xxxxx + SEG_D + SEG_E + SEG_F + SEG_G,
                self.CHAR_F         : SEG_A + xxxxx + xxxxx + xxxxx + SEG_E + SEG_F + SEG_G,
                self.CHAR_G         : SEG_A + xxxxx + SEG_C + SEG_D + SEG_E + SEG_F + xxxxx,
                self.CHAR_H         : xxxxx + SEG_B + SEG_C + xxxxx + SEG_E + SEG_F + SEG_G,
                self.CHAR_I         : SEG_A + xxxxx + xxxxx + SEG_D + SEG_E + xxxxx + xxxxx,
                self.CHAR_J         : SEG_A + xxxxx + SEG_C + SEG_D + xxxxx + xxxxx + xxxxx,
                self.CHAR_K         : xxxxx + SEG_B + xxxxx + xxxxx + SEG_E + SEG_F + SEG_G,
                self.CHAR_L         : xxxxx + xxxxx + xxxxx + SEG_D + SEG_E + SEG_F + xxxxx,
                self.CHAR_M         : SEG_A + xxxxx + SEG_C + xxxxx + SEG_E + xxxxx + SEG_G,
                self.CHAR_N         : xxxxx + xxxxx + SEG_C + xxxxx + SEG_E + xxxxx + SEG_G,
                self.CHAR_O         : SEG_A + xxxxx + SEG_C + SEG_D + SEG_E + xxxxx + SEG_G,
                self.CHAR_P         : SEG_A + SEG_B + xxxxx + xxxxx + SEG_E + SEG_F + SEG_G,
                self.CHAR_Q         : SEG_A + SEG_B + SEG_C + xxxxx + xxxxx + SEG_F + SEG_G,
                self.CHAR_R         : xxxxx + xxxxx + xxxxx + xxxxx + SEG_E + xxxxx + SEG_G,
                self.CHAR_S         : SEG_A + xxxxx + SEG_C + SEG_D + xxxxx + SEG_F + SEG_G,
                self.CHAR_T         : xxxxx + xxxxx + xxxxx + SEG_D + SEG_E + SEG_F + SEG_G,
                self.CHAR_U         : xxxxx + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F + xxxxx,
                self.CHAR_V         : xxxxx + xxxxx + SEG_C + SEG_D + SEG_E + xxxxx + xxxxx,
                self.CHAR_W         : xxxxx + SEG_B + xxxxx + SEG_D + xxxxx + SEG_F + SEG_G,
                self.CHAR_X         : SEG_A + xxxxx + SEG_C + SEG_D + SEG_E + xxxxx + xxxxx,
                self.CHAR_Y         : xxxxx + SEG_B + SEG_C + SEG_D + xxxxx + SEG_F + SEG_G,
                self.CHAR_Z         : SEG_A + xxxxx + xxxxx + SEG_D + xxxxx + xxxxx + SEG_G,
                self.CHAR_HYPHEN    : xxxxx + xxxxx + xxxxx + xxxxx + xxxxx + xxxxx + SEG_G,
                self.CHAR_NONE      : xxxxx + xxxxx + xxxxx + xxxxx + xxxxx + xxxxx + xxxxx,
        }
        self.hardware1 = hardware1
        self.hardware2 = hardware2

    def displayRaw(self, left_segment=None, right_segment=None):
        self.hardware1.display(left_segment)
        self.hardware2.display(right_segment)

    def display(self, left_char=None, right_char=None):
        left_char = self.convertStringCharToDisplayChar(left_char)
        right_char = self.convertStringCharToDisplayChar(right_char)
        self.hardware1.display(self.dic[left_char])
        self.hardware2.display(self.dic[right_char])

    def convertStringCharToDisplayChar(self, char):
        dic = {
                '0' : self.CHAR_0,
                '1' : self.CHAR_1,
                '2' : self.CHAR_2,
                '3' : self.CHAR_3,
                '4' : self.CHAR_4,
                '5' : self.CHAR_5,
                '6' : self.CHAR_6,
                '7' : self.CHAR_7,
                '8' : self.CHAR_8,
                '9' : self.CHAR_9,
                'A' : self.CHAR_A,
                'B' : self.CHAR_B,
                'C' : self.CHAR_C,
                'D' : self.CHAR_D,
                'E' : self.CHAR_E,
                'F' : self.CHAR_F,
                'G' : self.CHAR_G,
                'H' : self.CHAR_H,
                'I' : self.CHAR_I,
                'J' : self.CHAR_J,
                'K' : self.CHAR_K,
                'L' : self.CHAR_L,
                'M' : self.CHAR_M,
                'N' : self.CHAR_N,
                'O' : self.CHAR_O,
                'P' : self.CHAR_P,
                'Q' : self.CHAR_Q,
                'R' : self.CHAR_R,
                'S' : self.CHAR_S,
                'T' : self.CHAR_T,
                'U' : self.CHAR_U,
                'V' : self.CHAR_V,
                'W' : self.CHAR_W,
                'X' : self.CHAR_X,
                'Y' : self.CHAR_Y,
                'Z' : self.CHAR_Z,
                '-' : self.CHAR_HYPHEN,
                ''  : self.CHAR_NONE,
                }
        return dic[char]

class SsdMode:
    def __init__(self):
        pass
    def onEntry(self):
        pass
    def onExit(self):
        pass
    def onTick(self):
        pass

class DisplayPattern:
    def __init__(self, displayFrames):
        self.pictures = collections.deque()
        for p in pictures:
            self.pictures.append(p)

    def getNextPicture(self):
        p = self.pictures.popleft()
        self.pictures.append(p)
        return p

class DisplayFrame:
    def __init__(self, picture, timeMillisec):
        self.picture = picture
        self.timeMillisec = timeMillisec

    def getPicture(self):
        return self.picture

    def getTimeMillisec(self):
        return self.timeMillisec

class SsdTestMode(SsdMode):
    def __init__(self, ssdDriver):
        self.testSequence = [
                SsdDriver.SEG_F,
                SsdDriver.SEG_A,
                SsdDriver.SEG_B,
                SsdDriver.SEG_C,
                SsdDriver.SEG_D,
                SsdDriver.SEG_E,
                SsdDriver.SEG_G,
                SsdDriver.SEG_DP,
                ]
        self.currentIndex = 0
        self.ssdDriver = ssdDriver

    def onTick(self):
        i = self.currentIndex % len(self.testSequence)
        s = self.testSequence[i]
        self.ssdDriver.displayRaw(s, s)
        self.currentIndex += 1

class NormalSsdMode(SsdMode):
    def __init__(self, ssdDriver, stateManager, configManager, eventManager, loggingManager):
        self.ssdDriver = ssdDriver
        self.stateManager = stateManager
        self.configManager = configManager
        self.eventManager = eventManager
        self.loggingManager = loggingManager
        self.updateState()
        self.stateManager.subscribe(self.updateState)
        self.configManager.subscribe(self.updateState)
        self.eventManager.subscribe(self.updateState)
        self.loggingManager.subscribe(self.updateDotState)
        self.on = True

    def updateState(self):
        if self.eventManager.hasCriticalFault():
            self.state = self.CRITICAL_FAULT
        else:
            s = self.stateManager.getState()
            if s == StateManager.DEBUG:
                self.state = self.DEBUG
            elif s == StateManager.RUN:
                self.state = self.RUN
            else:
                self.state = self.PROGRAM

    def updateDotState(self):
        s = self.loggingManager.getState()
        if s == LoggingManager.NO_LOGGING_SETTINGS:
            #self.dot.
            pass

    def onTick(self):
        if self.state == self.PROGRAM:
            if self.on:
                self.ssdDriver.display('-', '-')
                self.on = False
            else:
                self.ssdDriver.display('', '')
                self.on = True
        elif self.state == self.DEBUG:
            self.ssdDriver.display('-', '-')
        elif self.state == self.RUN:
            sign = self.configManager.getSignature()
            if sign != None:
                self.ssdDriver.display(sign[2], sign[3])
            else:
                self.ssdDriver.display('-', '-')
        else:
            #elif self.state == self.CRITICAL_FAULT:
            self.ssdDriver.display('H', '3')

    PROGRAM = StateManager.PROGRAM
    DEBUG = StateManager.DEBUG
    RUN = StateManager.RUN
    CRITICAL_FAULT = EventManager.CRITICAL_FAULT

class IncrementalSsdMode(SsdMode):
    def __init__(self, ssdDriver):
        SsdMode.__init__(self)
        self.states = self.getInitialStates(ssdDriver)
        self.ssdDriver = ssdDriver

    def getInitialStates(self, ssdDriver):
        return collections.deque([
            '0',
            '1',
            '2',
            '3',
            '4',
            '5',
            '6',
            '7',
            '8',
            '9',
            'A',
            'B',
            'C',
            'D',
            'E',
            'F',
            '-',
        ])

    def onTick(self):
        s = self.states.popleft()
        self.ssdDriver.display(s, s)
        self.states.append(s)

    def onExit(self):
        self.states = self.getInitialStates(self.ssdDriver)

class DecrementalSsdMode(IncrementalSsdMode):
    def __init__(self, ssdDriver):
        IncrementalSsdMode.__init__(self, ssdDriver)

    def onTick(self):
        s = self.states.pop()
        self.ssdDriver.display(s, s)
        self.states.appendleft(s)

class SsdModeManager:
    def __init__(self, ssdDriver, timer, stateManager, configManager, eventManager):
        self.incrementalSsdMode = IncrementalSsdMode(ssdDriver)
        self.decrementalSsdMode = DecrementalSsdMode(ssdDriver)
        self.normalSsdMode = NormalSsdMode(ssdDriver, stateManager, configManager, eventManager)
        self.ssdTestMode = SsdTestMode(ssdDriver)
        #self.mode = self.ssdTestMode
        self.mode = self.normalSsdMode
        self.tickTimeSec = 1
        self.timer = timer
        self.timer.after(self.tickTimeSec * 1000, self.onTick)

    def onTick(self):
        self.mode.onTick()
        self.timer.after(self.tickTimeSec * 1000, self.onTick)

    def onEvent(self):
        m = self.getNextMode()
        if m != self.mode:
            self.mode.onExit()
            self.mode = m
            self.mode.onEntry()

    def getNextMode(self):
        if self.mode == self.incrementalSsdMode:
            return self.decrementalSsdMode
        else:
            return self.incrementalSsdMode

class ServiceSwitchButton(Button):
    def __init__(self, master=None):
        Button.__init__(self, master)
        self.pack()
        self.configure()

    def configure(self):
        self.isPressed = False
        self.bind("<ButtonPress-1>", self.buttonPress1Callback)
        self.bind("<ButtonRelease-1>", self.buttonRelease1Callback)

    def buttonPress1Callback(self, event):
        self.isPressed = True

    def buttonRelease1Callback(self, event):
        self.isPressed = False

    def isOn(self):
        return self.isPressed

class ServiceSwitchDriver:
    def __init__(self, hardware, timer):
        self.hardware = hardware
        self.timer = timer
        self.waiterForOn = []
        self.waiterForOff = []
        self.pollingTimeMilliSec = 100
        self.timeDuringSwitchIsOn = 0
        self.timer.after(self.pollingTimeMilliSec, self.poll)

    TIME     = 'time'
    CALLBACK    = 'callback'

    def poll(self):
        if self.hardware.isOn():
            self.timeDuringSwitchIsOn += self.pollingTimeMilliSec
            for w in self.waiterForOn:
                if w[self.TIME] <= self.timeDuringSwitchIsOn:
                    w[self.CALLBACK]()
        else:
            if self.timeDuringSwitchIsOn > 0:
                for w in self.waiterForOff:
                    w[self.CALLBACK]()
            self.timeDuringSwitchIsOn = 0
        self.timer.after(self.pollingTimeMilliSec, self.poll)

    def bindSwitchOn(self, on_time_millisec, callback):
        self.waiterForOn.append({self.TIME : on_time_millisec, self.CALLBACK : callback})
        self.waiterForOn.sort(key=lambda w: w[self.TIME])

    def bindSwitchOff(self, callback):
        self.waiterForOff.append({self.CALLBACK : callback})

class ServiceSwitchServiceOrganizer:
    def __init__(self, ssDriver, ssdModeManager):
        self.ssdModeManager = ssdModeManager
        self.ssDriver = ssDriver
        self.ssDriver.bindSwitchOff(self.onClick)

    def onClick(self):
        self.ssdModeManager.onEvent()

class StateSelectRadioButton:
    INITIALIZING = 'INITIALIZING'
    PROGRAM = 'PROGRAM'
    DEBUG = 'DEBUG'
    RUN = 'RUN'
    def __init__(self, master):
        MODES = [
            ('INITIALIZING', '1'),
            ('PROGRAM',      '2'),
            ('DEBUG',        '3'),
            ('RUN',          '4'),
            ]
        v = StringVar()
        # https://stackoverflow.com/questions/6548837/how-do-i-get-an-event-callback-when-a-tkinter-entry-widget-is-modified
        v.trace("w", lambda name, index, mode, sv=v: self.callback(sv))
        v.set('1')

        for text, mode in MODES:
            b = Radiobutton(master, text=text, variable=v, value=mode)
            b.pack(anchor=W)
        self.v = v

    def callback(self, sv):
        #sv.get()
        pass

    def getState(self):
        v = self.v.get()
        if v == '1':
            return self.INITIALIZING
        elif v == '2':
            return self.PROGRAM
        elif v == '3':
            return self.DEBUG
        else:
            return self.RUN

class LoggingSelectRadioButton:
    NO_LOGGING_SETTINGS = 'NO_LOGGING_SETTINGS'
    WAITING_FOR_TRIGGER = 'WAITING_FOR_TRIGGER'
    TRIGGERRED = 'TRIGGERRED'
    COMPLETED = 'COMPLETED'
    def __init__(self, master):
        MODES = [
            ('NO_LOGGING_SETTINGS', '1'),
            ('WAITING_FOR_TRIGGER', '2'),
            ('TRIGGERRED',          '3'),
            ('COMPLETED',           '4'),
            ]
        v = StringVar()
        # https://stackoverflow.com/questions/6548837/how-do-i-get-an-event-callback-when-a-tkinter-entry-widget-is-modified
        v.trace("w", lambda name, index, mode, sv=v: self.callback(sv))
        v.set('1')

        for text, mode in MODES:
            b = Radiobutton(master, text=text, variable=v, value=mode)
            b.pack(anchor=W)
        self.v = v

    def callback(self, sv):
        #sv.get()
        pass

    def getState(self):
        v = self.v.get()
        if v == '1':
            return self.NO_LOGGING_SETTINGS
        elif v == '2':
            return self.WAITING_FOR_TRIGGER
        elif v == '3':
            return self.TRIGGERRED
        else:
            return self.COMPLETED

class Application(Frame):
    def say_hi(self):
        print "hi threre, everyone!"

    def createWidgets(self):
        self.QUIT = Button(self)
        self.QUIT['text'] = 'QUIT'
        self.QUIT['fg'] = 'red'
        self.QUIT['command'] = self.quit

        self.QUIT.pack({'side': 'left'})

        self.hi_there = Button(self)
        self.hi_there['text'] = 'Hello'
        self.hi_there['command'] = self.say_hi

        self.hi_there.pack({'side' : 'left'})

        stateRadioButton = StateSelectRadioButton(self)
        loggingRadioButton = LoggingSelectRadioButton(self)

        stateManager = StateManager(radioButton, self)
        configManager = ConfigManager()
        eventManager = EventManager()
        loggingManager = LoggingManager(loggingRadioButton)
        ssdDriver = SsdDriver(SsdCanvas(self), SsdCanvas(self))
        self.ssdModeManager = SsdModeManager(ssdDriver, self, stateManager, configManager, eventManager)
        self.ssServiceOrganizer = ServiceSwitchServiceOrganizer(ServiceSwitchDriver(ServiceSwitchButton(self), self), self.ssdModeManager)

    def __init__(self, master=None):
        Frame.__init__(self, master)
        self.pack()
        self.createWidgets()

root = Tk()
app = Application(master=root)
app.mainloop()
root.destroy()

