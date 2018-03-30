/*
  Put the noduino logo on the display.
  
  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
*/

#include "noduino.h"
#include "U8glib.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C / TWI 
//#define MINI_LOGO

void drawColorBox(void)
{
	u8g_uint_t w, h;
	u8g_uint_t r, g, b;

	w = u8g.getWidth() / 32;
	h = u8g.getHeight() / 8;
	for (b = 0; b < 4; b++)
		for (g = 0; g < 8; g++)
			for (r = 0; r < 8; r++) {
				u8g.setColorIndex((r << 5) | (g << 2) | b);
				u8g.drawBox(g * w + b * w * 8, r * h, w, h);
			}
}

void drawLogo(uint8_t d)
{
	//u8g.setFont(u8g_font_freedoomr10r);
	//u8g.setFont(u8g_font_agencyfb32ptr);
	u8g.setFont(u8g_font_rajdhani_semibold32ptr);
	//u8g.setScale2x2();    // Scale up all draw procedures
	u8g.drawStr(15 + d, 26 + d, "Noduino");
	//u8g.undoScale();      // Scale up all draw procedures

	u8g.drawHLine(48 + d, 35 + d, 73);
	u8g.drawVLine(48 + d, 34 + d, 3);
}

void drawURL(void)
{
	//u8g.setFont(u8g_font_agencyfb9ptr);
	u8g.setFont(u8g_font_4x6);
	u8g.drawStr(50, 48, "http://noduino.org");
}

void draw(void)
{
	if (u8g.getMode() == U8G_MODE_R3G3B2) {
		drawColorBox();
	}
	u8g.setColorIndex(1);
	if (U8G_MODE_GET_BITS_PER_PIXEL(u8g.getMode()) > 1) {
		drawLogo(2);
		u8g.setColorIndex(2);
		drawLogo(1);
		u8g.setColorIndex(3);
	}
	drawLogo(0);
	drawURL();

}

void setup(void)
{
	//flip screen, if required
	//u8g.setRot180();
}

void loop(void)
{
	// picture loop
	u8g.firstPage();
	do {
		draw();
		u8g.setColorIndex(1);
	} while (u8g.nextPage());

	// rebuild the picture after some delay
	delay(200);
}
