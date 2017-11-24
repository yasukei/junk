#!/usr/bin/env python
# -*- config: utf-8 -*-

# refer: http://docs.python.jp/2/library/tkinter.html

from Tkinter import *
import collections
from threading import Timer

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
    segments = [SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G]

class SsdDriver:
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
    CHAR_NONE = '-'

    def __init__(self, hardware):
        SEG_A   = hardware.SEG_A 
        SEG_B   = hardware.SEG_B 
        SEG_C   = hardware.SEG_C 
        SEG_D   = hardware.SEG_D 
        SEG_E   = hardware.SEG_E 
        SEG_F   = hardware.SEG_F 
        SEG_G   = hardware.SEG_G 
        SEG_DP  = hardware.SEG_DP
        xxxxx   = ''
        self.dic = {
                self.CHAR_0     : SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F + xxxxx,
                self.CHAR_1     : xxxxx + SEG_B + SEG_C + xxxxx + xxxxx + xxxxx + xxxxx,
                self.CHAR_2     : SEG_A + SEG_B + xxxxx + SEG_D + SEG_E + xxxxx + SEG_G,
                self.CHAR_3     : SEG_A + SEG_B + SEG_C + SEG_D + xxxxx + xxxxx + SEG_G,
                self.CHAR_4     : xxxxx + SEG_B + SEG_C + xxxxx + xxxxx + SEG_F + SEG_G,
                self.CHAR_5     : SEG_A + xxxxx + SEG_C + SEG_D + xxxxx + SEG_F + SEG_G,
                self.CHAR_6     : SEG_A + xxxxx + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G,
                self.CHAR_7     : SEG_A + SEG_B + SEG_C + xxxxx + xxxxx + xxxxx + xxxxx,
                self.CHAR_8     : SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G,
                self.CHAR_9     : SEG_A + SEG_B + SEG_C + SEG_D + xxxxx + SEG_F + SEG_G,
                self.CHAR_A     : SEG_A + SEG_B + SEG_C + xxxxx + SEG_E + SEG_F + SEG_G,
                self.CHAR_B     : xxxxx + xxxxx + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G,
                self.CHAR_C     : SEG_A + xxxxx + xxxxx + SEG_D + SEG_E + SEG_F + xxxxx,
                self.CHAR_D     : xxxxx + SEG_B + SEG_C + SEG_D + SEG_E + xxxxx + SEG_G,
                self.CHAR_E     : SEG_A + xxxxx + xxxxx + SEG_D + SEG_E + SEG_F + SEG_G,
                self.CHAR_F     : SEG_A + xxxxx + xxxxx + xxxxx + SEG_E + SEG_F + SEG_G,
                self.CHAR_NONE  : xxxxx + xxxxx + xxxxx + xxxxx + xxxxx + xxxxx + SEG_G,
        }
        self.hardware = hardware

    def display(self, char=None):
        self.hardware.display(self.dic[char])

class SsdMode:
    def __init__(self):
        pass
    def onEntry(self):
        pass
    def onExit(self):
        pass
    def onTick(self):
        pass

class NormalSsdMode(SsdMode):
    def __init__(self, ssdDriver):
        SsdMode.__init__(self)
        self.states = collections.deque([
            ssdDriver.CHAR_0,
            ssdDriver.CHAR_1,
            ssdDriver.CHAR_2,
            ssdDriver.CHAR_3,
            ssdDriver.CHAR_4,
            ssdDriver.CHAR_5,
            ssdDriver.CHAR_6,
            ssdDriver.CHAR_7,
            ssdDriver.CHAR_8,
            ssdDriver.CHAR_9,
            ssdDriver.CHAR_A,
            ssdDriver.CHAR_B,
            ssdDriver.CHAR_C,
            ssdDriver.CHAR_D,
            ssdDriver.CHAR_E,
            ssdDriver.CHAR_F,
            ssdDriver.CHAR_NONE,
        ])
        self.ssdDriver = ssdDriver

    def onTick(self):
        s = self.states.popleft()
        self.ssdDriver.display(s)
        self.states.append(s)

class ReversedNormalSsdMode(NormalSsdMode):
    def __init__(self, ssdDriver):
        NormalSsdMode.__init__(self, ssdDriver)

    def onTick(self):
        s = self.states.pop()
        self.ssdDriver.display(s)
        self.states.appendleft(s)

class SsdModeManager:
    def __init__(self, ssdDriver, timer):
        self.normalSsdMode = NormalSsdMode(ssdDriver)
        self.reversedNormalSsdMode = ReversedNormalSsdMode(ssdDriver)
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
        if self.mode == self.normalSsdMode:
            return self.reversedNormalSsdMode
        else:
            return self.normalSsdMode


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

        self.ssdModeManager = SsdModeManager(SsdDriver(SsdCanvas(self)), self)
        self.ssServiceOrganizer = ServiceSwitchServiceOrganizer(ServiceSwitchDriver(ServiceSwitchButton(self), self), self.ssdModeManager)

    def __init__(self, master=None):
        Frame.__init__(self, master)
        self.pack()
        self.createWidgets()

root = Tk()
app = Application(master=root)
app.mainloop()
root.destroy()
