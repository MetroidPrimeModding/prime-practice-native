#ifndef CGAMEAREA_HPP
#define CGAMEAREA_HPP

class CStateManager;

class CGameArea {
public:
    void StartStreamIn(CStateManager&);
    void Validate(CStateManager&);
    void LoadScriptObjects(CStateManager&);
};

#endif // CGAMEAREA_HPP