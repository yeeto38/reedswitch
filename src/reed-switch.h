#pragma once
#include <Arduino.h>

/*
  Reed Switch Manager
  - Reads a digital reed switch input
  - Debounces
  - Tracks "time in stable state == 1" (HIGH)
  - Pauses timer when state == 0 (LOW)
  - Resets timer when state transitions 0 -> 1
  - Prints only on changes (optional)
*/

class ReedSwitch {
public:
  // activeLow = true means: LOW => magnet present / triggered
  ReedSwitch(int gpioPin, bool activeLow = true, uint32_t debounceMs = 30)
    : _pin(gpioPin), _activeLow(activeLow), _debounceMs(debounceMs) {}

  void begin(bool useInternalPullup = true) {
    if (useInternalPullup) {
      pinMode(_pin, INPUT_PULLUP);
    } else {
      pinMode(_pin, INPUT);
    }

    _stableRaw = digitalRead(_pin);
    _lastRaw = _stableRaw;
    _lastRawChangeMs = millis();

    // Start timer logic based on initial state
    _state = normalizedState(_stableRaw);

    _lastStateChangeMs = millis();
    if (_state == 1) {
      _timerRunning = true;
      _startMs = millis();
      _elapsedBeforePauseMs = 0;
    } else {
      _timerRunning = false;
      _startMs = 0;
      _elapsedBeforePauseMs = 0;
    }
  }

  // Call frequently in loop()
  void update() {
    int raw = digitalRead(_pin);

    if (raw != _lastRaw) {
      _lastRaw = raw;
      _lastRawChangeMs = millis();
    }

    // Debounce: accept new stable value if it remains stable for _debounceMs
    if ((millis() - _lastRawChangeMs) >= _debounceMs) {
      if (raw != _stableRaw) {
        _stableRaw = raw;

        int newState = normalizedState(_stableRaw);
        if (newState != _state) {
          // state change
          int oldState = _state;
          _state = newState;
          _lastStateChangeMs = millis();

          // State machine:
          // - When going to 0: pause timer (freeze)
          // - When going to 1: reset timer to 0 and start counting
          if (_state == 0) {
            pauseTimer();
          } else { // _state == 1
            resetAndStartTimer();
          }

          // Serial logging only on changes
          if (_serialLogging) {
            Serial.print("[REED] State changed: ");
            Serial.print(oldState);
            Serial.print(" -> ");
            Serial.println(_state);
          }
        }
      }
    }
  }

  void enableSerialLogging(bool enable) { _serialLogging = enable; }

  // Normalized state:
  // user wants: 1 means "normal / magnet away"; 0 means "magnet present"
  int getState() const { return _state; }

  // Milliseconds elapsed while stable at state==1 since last reset
  uint32_t getElapsedMs() const {
    if (_state != 1) {
      // frozen
      return _elapsedBeforePauseMs;
    }
    if (!_timerRunning) return _elapsedBeforePauseMs;
    return _elapsedBeforePauseMs + (millis() - _startMs);
  }

  // Helpful for UI
  bool isPaused() const { return (_state == 0); }
  uint32_t lastStateChangeMs() const { return _lastStateChangeMs; }

private:
  int _pin;
  bool _activeLow;
  uint32_t _debounceMs;

  int _stableRaw = HIGH;
  int _lastRaw = HIGH;
  uint32_t _lastRawChangeMs = 0;

  int _state = 1; // normalized: 1=not triggered, 0=triggered
  uint32_t _lastStateChangeMs = 0;

  // Timer tracking
  bool _timerRunning = false;
  uint32_t _startMs = 0;
  uint32_t _elapsedBeforePauseMs = 0;

  bool _serialLogging = true;

  int normalizedState(int raw) const {
    // If activeLow:
    // raw HIGH => no magnet => state 1
    // raw LOW  => magnet => state 0
    if (_activeLow) {
      return (raw == LOW) ? 0 : 1;
    } else {
      return (raw == HIGH) ? 0 : 1;
    }
  }

  void pauseTimer() {
    if (_timerRunning) {
      _elapsedBeforePauseMs += (millis() - _startMs);
      _timerRunning = false;
    }
  }

  void resetAndStartTimer() {
    _elapsedBeforePauseMs = 0;
    _startMs = millis();
    _timerRunning = true;
  }
};
