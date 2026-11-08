#pragma once
#include "VisualNode.h"

using namespace sf;

class LayoutEngine {
public:
    void calculatePositions( VisualNode* root,float windowWidth, float topY = 80.f, float levelGap = 100.f);

private:
    void assignPositions(VisualNode* node, float leftX, float rightX,float topY, float levelGap, int depth);
};