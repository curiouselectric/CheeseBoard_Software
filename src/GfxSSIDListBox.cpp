#include <MutilaDebug.h>
#include <Millis.h>
#include "GfxSSIDListBox.h"
#include "GfxTextBox.h"
#include "GfxSignalStrength.h"
#include "CbOledDisplay.h"
#include "CheeseboardConfig.h"

GfxSSIDListBox::GfxSSIDListBox(uint16_t x, uint16_t y) :
    _selected(-1)
{
    for(uint8_t i=0; i<MaxItems; i++) {
        _items[i] = NULL;
    }

    // TODO: calculate screen lines based on line height and screen size
    // as ooposed to this hard-coded value
    _lineHeight = (CBOLED_MESSAGE_FONT_HEIGHT + (2*CBOLED_MESSAGE_FONT_VSEP));
    _screenLines = CbOledDisplay.getDisplayHeight() / _lineHeight;
    _screenStart = 0;
}

GfxSSIDListBox::~GfxSSIDListBox()
{
    for(uint8_t i=0; i<MaxItems; i++) {
        if (_items[i] != NULL) {
            delete _items[i];
            _items[i] = NULL;
        }
    }
}

int16_t GfxSSIDListBox::add(const GfxNetInfo s) 
{
    int16_t i = findGap();
    if (i >= 0) {
        _items[i] = new GfxNetInfo(s);
        return true;
    } else {
        return false;
    }
}

bool GfxSSIDListBox::remove(const uint8_t idx, bool compactAfter)
{
    if (idx >= 0 && idx <MaxItems && _items[idx] != NULL) {
        delete _items[idx];
        _items[idx] = NULL;

        // remove selection
        if (_selected == idx) {
            _selected = -1;
        }

        // compact if requested
        if (compactAfter) {
            compact();
        }
        return true;
    } else {
        return false;
    }
}

void GfxSSIDListBox::draw(uint16_t xOffset, uint16_t yOffset) 
{
    uint8_t drawn = 0;
    for(uint8_t i=0; i<MaxItems && drawn<_screenLines; i++) {
        if (_items[i] != NULL) {
            if (isOnScreen(i)) {
                uint16_t y = drawn * _items[i]->height();
                _items[i]->draw(xOffset, yOffset+y);
                if (_selected == i) {
                    CbOledDisplay.drawFrame(xOffset, y, _items[i]->width(), _items[i]->height());
                }
                drawn++;
            }
        }
    }
}

uint16_t GfxSSIDListBox::width()
{
    return 128;
}

uint16_t GfxSSIDListBox::height()
{
    return _lineHeight * (count() < _screenLines ? count() : _screenLines);
}

GfxNetInfo* GfxSSIDListBox::operator[](int16_t idx)
{
    if (idx < 0 or idx >= MaxItems) {
        return NULL;
    } else {
        return _items[idx];
    }
}

GfxNetInfo* GfxSSIDListBox::operator[](const String& ssid)
{
    int16_t idx = this->find(ssid, 1);
    if (idx == -1) {
        return NULL;
    } else {
        return _items[idx];
    }
}

int16_t GfxSSIDListBox::findGap(uint8_t startAt)
{
    uint8_t i;
    for(i=startAt; i<MaxItems && _items[i] != NULL; i++) {;}
    if (i<MaxItems) {
        return i;
    } else {
        return -1;
    }
}

int16_t GfxSSIDListBox::findNext(uint8_t from)
{
    for(uint8_t i=from+1; i<MaxItems; i++) {
        if (_items[i] != NULL) {
            return i;
        }
    }
    return -1;
}

int16_t GfxSSIDListBox::findPreceding(uint8_t from)
{
    if (from == 0 || from >= MaxItems) {
        return -1;
    }

    for(uint8_t i=from-1; i>=0; i--) {
        if (_items[i] != NULL) {
            return i;
        }
    }
    return -1;
}

uint8_t GfxSSIDListBox::count()
{
    uint8_t count=0;
    for (uint8_t i=0; i<MaxItems; i++) {
        if (_items[i] != NULL) {
            count++;
        }
    }
    return count;
}

int16_t GfxSSIDListBox::find(const String& ssid, uint8_t n)
{
    for (uint8_t i=0; i<MaxItems; i++) {
        if (_items[i] != NULL) {
            if (_items[i]->ssid() == ssid) {
                if (n-- == 1) {
                    return i;
                }
            }
        }
    }
    return -1;
}

//! Sort the items contained in the list box using quicksort
// TODO: void GfxSSIDListBox::sort();

bool GfxSSIDListBox::isCompacted()
{
    int16_t gap = findGap();
    if (gap >= 0) {
        for (uint8_t i=gap+1; i<MaxItems; i++) {
            if (_items[i] != NULL) {
                return false;
            }
        }
    }
    return true;
}

//! Make used items contiguous (remove gaps)
void GfxSSIDListBox::compact()
{
    while (!isCompacted()) {
        int16_t gap = findGap();
        if (gap >= 0) {
            for(uint8_t i=gap; i<MaxItems-1; i++) {
                _items[i] = _items[i+1];
                if (i+1 == _selected) {
                    _selected = i;
                }
            }
            _items[MaxItems-1] = NULL;
        }
    }
}

bool GfxSSIDListBox::select(int16_t idx)
{
    if (idx >= MaxItems || !(idx == -1 || _items[idx] != NULL)) {
        return false;
    } else {
        _selected = idx;
        return true;
        return false;
    }
}

//! \return the index of the currently selected item (or -1 if none are selected)
int16_t GfxSSIDListBox::selected()
{
    return _selected;
}

bool GfxSSIDListBox::isOnScreen(uint8_t idx)
{
    uint8_t lines = _screenLines;
    for (uint8_t i=_screenStart; i<MaxItems && lines>0; i++) {
        if (i == idx) {
            return true;
        }
        if (_items[i] != NULL) {
            lines--;
        }
    }
    return false;
}

uint8_t GfxSSIDListBox::rowsFrom(uint8_t idx)
{
    uint8_t count = 0;
    for (uint8_t i=idx; i<MaxItems; i++) {
        if (_items[i] != NULL) {
            count++;
        }
    }
    return count;
}

uint8_t GfxSSIDListBox::scrollDown(uint8_t rows)
{
    uint8_t scrolled = 0;
    while(rows > 0) {
        int16_t nextItem = findNext(_screenStart);
        if (nextItem == -1) { 
            return scrolled; 
        }
        if (rowsFrom(nextItem) < _screenLines) {
            return scrolled;
        } else {
            _screenStart = nextItem;
            scrolled++;
        }
        rows--;
    }
    return scrolled;
}

uint8_t GfxSSIDListBox::scrollUp(uint8_t rows)
{
    uint8_t scrolled = 0;
    while(rows > 0) {
        int16_t preItem = findPreceding(_screenStart);
        if (preItem == -1) { 
            return scrolled; 
        }
        _screenStart = preItem;
        scrolled++;
        rows--;
    }
    return scrolled;
}

bool GfxSSIDListBox::scrollTo(uint8_t idx)
{
    if (_items[idx] == NULL || idx > MaxItems) {
        return false;
    }

    if (idx < _screenStart) {
        // Ohhhh... I feel like I'm tempting an infinite loop here
        while(!isOnScreen(idx)) {
            scrollUp(1);
        }
    } else {
        while(!isOnScreen(idx)) {
        // Ohhhh... I feel like I'm tempting an infinite loop here
            scrollDown(1);
        }
    }
    return true;
}

int16_t GfxSSIDListBox::update(String ssid, int8_t channel, uint8_t signal)
{
    int16_t idx = this->find(ssid);
    if (idx != -1) {
        _items[idx]->update(channel, signal);
        return idx;
    } else {
        GfxNetInfo* netInfo = new GfxNetInfo(ssid, channel, signal);
        if (netInfo == NULL) {
            // ERROR, could not allocate new object
            return -1;
        }
        idx = this->add(*netInfo);
        if (idx == -1) {
            // ERROR could not add item to list
            delete netInfo;
        }
        return idx;
    }
}

