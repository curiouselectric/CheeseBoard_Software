#include <MutilaDebug.h>
#include "GfxStringListBox.h"
#include "GfxTextBox.h"
#include "CheeseboardConfig.h"

GfxStringListBox::GfxStringListBox(uint16_t width) :
    _width(width),
    _lineHeight(CBOLED_MESSAGE_FONT_HEIGHT + (2*CBOLED_MESSAGE_FONT_VSEP)),
    _selected(-1)
{
    for(uint8_t i=0; i<MaxItems; i++) {
        _items[i] = NULL;
    }

    // TODO: calculate screen lines based on line height and screen size
    // as ooposed to this hard-coded value
    _screenLines = 4;
    _screenStart = 0;
}

GfxStringListBox::~GfxStringListBox()
{
    for(uint8_t i=0; i<MaxItems; i++) {
        if (_items[i] != NULL) {
            delete _items[i];
            _items[i] = NULL;
        }
    }
}

int16_t GfxStringListBox::add(const String s) 
{
    int16_t i = findGap();
    if (i >= 0) {
        _items[i] = new String(s);
        return true;
    } else {
        return false;
    }
}

bool GfxStringListBox::remove(const uint8_t idx, bool compactAfter)
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

void GfxStringListBox::draw(uint16_t xOffset, uint16_t yOffset) 
{
    DBLN(F("GfxStringListBox::draw"));
    uint8_t drawn = 0;
    for(uint8_t i=0; i<MaxItems && drawn<_screenLines; i++) {
        if (_items[i] != NULL) {
            if (isOnScreen(i)) {
                GfxTextBox tb(_width, *_items[i], selected() == i);
                tb.draw(xOffset, yOffset+(tb.height()*drawn));
                drawn++;
            }
        }
    }
}

uint16_t GfxStringListBox::width()
{
    return _width;
}

uint16_t GfxStringListBox::height()
{
    return _lineHeight * _screenLines;
}

int16_t GfxStringListBox::findGap(uint8_t startAt)
{
    uint8_t i;
    for(i=startAt; i<MaxItems && _items[i] != NULL; i++) {;}
    if (i<MaxItems) {
        return i;
    } else {
        return -1;
    }
}

int16_t GfxStringListBox::findNext(uint8_t from)
{
    for(uint8_t i=from+1; i<MaxItems; i++) {
        if (_items[i] != NULL) {
            return i;
        }
    }
    return -1;
}

int16_t GfxStringListBox::findPreceding(uint8_t from)
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

int16_t GfxStringListBox::find(const String s, uint8_t n)
{
    uint8_t i;
    for(i=0; i<MaxItems; i++) {
        if (*_items[i] == s) {
            if (n-- == 1) {
                return i;
            }
        }
    }
    return -1;
}

//! Sort the items contained in the list box using quicksort
// TODO: void GfxStringListBox::sort();

bool GfxStringListBox::isCompacted()
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
void GfxStringListBox::compact()
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

bool GfxStringListBox::select(int16_t idx)
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
int16_t GfxStringListBox::selected()
{
    return _selected;
}

bool GfxStringListBox::isOnScreen(uint8_t idx)
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

uint8_t GfxStringListBox::rowsFrom(uint8_t idx)
{
    uint8_t count = 0;
    for (uint8_t i=idx; i<MaxItems; i++) {
        if (_items[i] != NULL) {
            count++;
        }
    }
    return count;
}

uint8_t GfxStringListBox::scrollDown(uint8_t rows)
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

uint8_t GfxStringListBox::scrollUp(uint8_t rows)
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

bool GfxStringListBox::scrollTo(uint8_t idx)
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



