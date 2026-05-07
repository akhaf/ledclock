#include "beeper.h"

static uint16_t singleBeep[3];

Beeper::Beeper(uint8_t channel, uint8_t pin, int8_t enablePin) : _channel(channel), _enablePin(enablePin), _beep_timer(0) {
    ledcAttachPin(pin, channel);

    if (_enablePin >= 0) {
        pinMode(_enablePin, OUTPUT);
        digitalWrite(_enablePin, LOW);
    }
}

void Beeper::_writeTone(uint16_t f) {
    if (f > 0) {
        if (_enablePin >= 0) {
            digitalWrite(_enablePin, HIGH);
        }

        ledcWriteTone(_channel, f);
    } else {
        ledcWriteTone(_channel, 0);

        if (_enablePin >= 0) {
            digitalWrite(_enablePin, LOW);
        }
    }
}

void Beeper::beep(uint16_t frequency, uint16_t duration) {
    if (duration == 0) {
        _writeTone(frequency);
    } else {
        singleBeep[0] = 1;
        singleBeep[1] = frequency;
        singleBeep[2] = duration;
        play(singleBeep);
    }
}

void Beeper::play(uint16_t* beep) {
    _current_beep = beep;
    _current_note = 0;
    _total_notes = *beep;
}

void Beeper::mute() {
    _writeTone(0);
    _current_beep = 0;
}

void Beeper::update() {
    if (_current_beep == 0 || !_beep_timer.fire()) {
        return;
    }

    if (_current_note < _total_notes) {
        uint16_t n = 2 * _current_note;
        uint16_t f = _current_beep[1 + n];
        uint16_t t = _current_beep[2 + n];
        _beep_timer.reset(t);
        _writeTone(f);
        _current_note++;
    } else {
        _writeTone(0);
        _current_beep = 0;
    }
}
