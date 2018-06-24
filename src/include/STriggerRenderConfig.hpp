#ifndef PRIME_PRACTICE_NATIVE_STRIGGERRENDERCONFIG_HPP
#define PRIME_PRACTICE_NATIVE_STRIGGERRENDERCONFIG_HPP

struct STriggerRenderConfig {
public:
    bool renderUnknown = false;
    bool renderLoad = false;
    bool renderDoor = false;
    bool renderForce = false;
    bool renderCameraHint = false;

    inline bool anyOn() {
      return renderUnknown
             || renderLoad
             || renderDoor
             || renderForce
             || renderCameraHint;
    }
};

#endif //PRIME_PRACTICE_NATIVE_STRIGGERRENDERCONFIG_HPP
