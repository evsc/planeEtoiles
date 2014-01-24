#pragma once

#include "ofMain.h"
#include "mediaElement.h"

class textElement: public mediaElement
{
    public:
        textElement();
        textElement(string t, string c);

        void update(float updateRate = 1.0);

        void setText(string t, string c);

        void draw(ofTrueTypeFont * font);
        void draw(ofTrueTypeFont * font, int x, int y);
        void drawElement(ofTrueTypeFont * font);

        string rawText;
        string compText;

        vector< string > textLine;
        vector< string > compLine;
        int textLineH;

    protected:
    private:
};
