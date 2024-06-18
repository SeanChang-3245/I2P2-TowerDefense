#ifndef DRAWBOARD_HPP
#define DRAWBOARD_HPP

#include <vector>
#include "Engine/Group.hpp"
#include "Image.hpp"
#include "ML_Macro.hpp"

class DrawBoard : public Engine::Group
{
private:
    enum PixelType { BLACK, WHITE, GRAY};
    bool mouse_holding;
    Group *PixelGroup;
    std::vector<std::vector<Engine::Image*>> pixel_ptr;
    std::vector<std::vector<PixelType>> pixel_status;

public:
    static const int PixelWidth;
    // Number of pixels on each side
    static const int PictureWidth;

    // top left corner coordinate
    DrawBoard(int x, int y);
    void OnMouseDown(int button, int mx, int my) override;
    void OnMouseUp(int button, int mx, int my) override;
    void OnMouseMove(int mx, int my) override;
    void DrawOnBoard(int x, int y);
    void DrawOnBoard(int x, int y, PixelType ty);


#if USE_ML
    int GetNumber() const;
#endif

};

#endif