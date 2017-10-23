#include <CbOledDisplay.h>
#include <RealTimeClock.h>
#include <EspApConfigurator.h>
#include <Millis.h>

#include "ClockDisplay.h"
#include "Config.h"

ClockDisplayClass ClockDisplay;

ClockDisplayClass::ClockDisplayClass() :
    _enabled(true),
    _modeLine(""),
    _updated(true),
    _lastTimeStr(""),
    _lastUpdate(0)
{
}

void ClockDisplayClass::begin()
{
}

void ClockDisplayClass::update()
{
    if (Millis() > _lastUpdate + RefreshMs) {
        _lastUpdate = Millis();

        if (!_enabled) { return; }

        String timeStr;

        if (RealTimeClock.haveRealTime()) {
            timeStr = RealTimeClock.timeStr(EspApConfigurator[SET_SHOW_SECONDS]->get().toInt());
        } else { 
            timeStr = F("I'm making time...");
        }
        if (timeStr != _lastTimeStr) {
            _updated = true;
            _lastTimeStr = timeStr;
        }

        if (_updated) {
            // clear update flag
            _updated = false;

            CbOledDisplay.clearBuffer();
            // TODO: draw time large, top, centre justified
            //
            // TODO: draw date under time, small
            //
            // TODO: draw mode line at bottom, centre justfied
            String text = timeStr;
            text += '\n';
            text += RealTimeClock.dateStr();
            text += '\n';
            text += '\n';
            text += _modeLine;
            CbOledDisplay.drawText(text.c_str(), 'C', 'M');
            CbOledDisplay.sendBuffer();
        }
    }
}

