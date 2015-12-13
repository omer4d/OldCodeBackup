#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <allegro.h>
#include <vector>
#include <stdexcept>
#include <map>

#include "Vec2f.hpp"
#include "Mouse.hpp"
#include "Keyboard.hpp"
#include "Lexcast.hpp"

#include "Brush.hpp"
#include "Workspace.hpp"

BITMAP* buffer;

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

void init()
{
	allegro_init();
	install_mouse();
	install_keyboard();
	set_color_depth(32);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
	buffer = create_bitmap(SCREEN_W, SCREEN_H);
	srand(time(NULL));
}

void deinit()
{
	destroy_bitmap(buffer);
}

class Cursor {
	Bitmap mBmp;
	int mTipX, mTipY;

	static BITMAP* loadCursor(char const* path)
	{
		BITMAP* bmp = load_bitmap(path, nullptr);
		
		if(!bmp)
			throw std::runtime_error("Failed to load cursor!");
			
		return bmp;
	}

public:
	Cursor(): mBmp(nullptr), mTipX(0), mTipY(0)
	{
	}

	Cursor(char const* path, int tipX, int tipY): mBmp(loadCursor(path)),
	mTipX(tipX), mTipY(tipY)
	
	{
	}
	
	void draw(BITMAP* dest, int x, int y) const
	{
		draw_sprite(dest, mBmp.bmp, x - mTipX, y - mTipY);
	}
};

class Editor {
	enum Tool {
		TOOL_PENCIL, TOOL_BRUSH, TOOL_FLOOD, TOOL_SAMPLE, TOOL_ARROW
	};
	
	enum CursorType {
		CUR_PENCIL, CUR_BRUSH, CUR_FLOOD, CUR_GENERIC, CUR_SAMPLE, CUR_NUM
	};
	
	static constexpr float PLAY_SPEED = 0.02f; 
	static constexpr float BRUSH_SCALE = 0.1;
	
	Keyboard mKeyboard;
	Mouse mMouse;
	Vec2f mLastMousePos;
	
	bool mEditing;
	Cursor mCursorArr[CUR_NUM];
	CursorType mCursorType;
	
	std::vector<Workspace> mWorkspaceVec;
	float mFramePos;
	
	float mBrushSize;
	Brush mBrush;
	Tool mTool;
	
	Cursor const& getCursor() const
	{
		return mCursorArr[mCursorType];
	}
	
	Workspace& getCurrWorkspace()
	{
		return mWorkspaceVec[int(mFramePos)];
	}
	
	void initCursors()
	{
		mCursorArr[CUR_GENERIC] = Cursor("DATA/generic.bmp", 8, 8);
		mCursorArr[CUR_SAMPLE] = Cursor("DATA/sample.bmp", 0, 15);
		mCursorArr[CUR_BRUSH] = Cursor("DATA/brush.bmp", 8, 8);
		mCursorArr[CUR_PENCIL] = Cursor("DATA/pencil.bmp", 0, 16);
		mCursorArr[CUR_FLOOD] = Cursor("DATA/flood.bmp", 3, 15);
	}
	
	void clampFramePos()
	{
		if(mFramePos < 0) mFramePos = 0;
		if(mFramePos > float(mWorkspaceVec.size() - 1)) mFramePos = mWorkspaceVec.size() - 1;
	}
	
public:
	Editor(): mEditing(false), mCursorType(CUR_BRUSH), mFramePos(0), mTool(TOOL_BRUSH), mBrushSize(10)
	{
		mBrush.setSize(int(mBrushSize));
		initCursors();
	}
	
	~Editor()
	{
	}
	
	void load(char const* pat, int min=0, int max=0)
	{
		char path[500];
		mWorkspaceVec.reserve(max - min + 1);
		
		for(int i = min; i <= max; ++i)
		{
			snprintf(path, 500, pat, i);
			printf("Loading %s\n", path);
			BITMAP* temp = load_bitmap(path, nullptr);
			
			if(!temp)
				throw std::runtime_error("File not found!");
			
			mWorkspaceVec.push_back(Workspace(temp));
			
			destroy_bitmap(temp);
		}
	}
	
	void handleMouseCmds()
	{
		if(mMouse.pressed(1))
		{
			for(auto& workspace: mWorkspaceVec)
				workspace.beginWrite();
			
			mLastMousePos.init(mouse_x, mouse_y);
			mEditing = true;
		}
		
		else if(mMouse.held(1))
		{
			switch(mTool)
			{
				case TOOL_BRUSH:
					for(auto& workspace: mWorkspaceVec)
					{
						workspace.brushStroke(mBrush, mLastMousePos, mMouse.getPos(),
												mBrush.getSize() / 3);
					}
					break;
					
				default:
					break;
			}
		}
		
		else if(mMouse.released(1))
		{
			for(auto& workspace: mWorkspaceVec)
				workspace.endWrite();
			mEditing = false;
		}
	}
	
	void handleKeyboardCmds()
	{
		if(mKeyboard.held(KEY_EQUALS))
			mBrush.grow(0.02);
		
		if(mKeyboard.held(KEY_MINUS))
			mBrush.grow(-0.02);
		
		if(mKeyboard.pressed(KEY_B))
		{
			mTool = TOOL_BRUSH;
			mCursorType = CUR_BRUSH;
		}
		
		if(mKeyboard.pressed(KEY_P))
		{
			mTool = TOOL_PENCIL;
			mCursorType = CUR_PENCIL;
		}
		
		if(mKeyboard.pressed(KEY_S))
		{
			mTool = TOOL_SAMPLE;
			mCursorType = CUR_SAMPLE;
		}
		
		if(mKeyboard.pressed(KEY_A))
		{
			mTool = TOOL_ARROW;
			mCursorType = CUR_GENERIC;
		}
		
		if(mKeyboard.pressed(KEY_E))
		{
			mTool = TOOL_BRUSH;
			mCursorType = CUR_BRUSH;
			mBrush.setCol(getCurrWorkspace().guessBgColor());
		}
		
		if(mKeyboard.pressed(KEY_LCONTROL, KEY_Z) && !mEditing)
		{
			for(auto& workspace: mWorkspaceVec)
				workspace.undo();
		}
		
		if(mKeyboard.pressed(KEY_LCONTROL, KEY_Y) && !mEditing)
		{
			for(auto& workspace: mWorkspaceVec)
				workspace.redo();
		}
		
		if(mKeyboard.pressed(KEY_LEFT))
		{
			--mFramePos;
			clampFramePos();
		}
		
		else if(mKeyboard.held(KEY_LSHIFT, KEY_LEFT))
		{
			mFramePos -= PLAY_SPEED;
			clampFramePos();
		}
		
		if(mKeyboard.pressed(KEY_RIGHT))
		{
			++mFramePos;
			clampFramePos();
		}
		
		else if(mKeyboard.held(KEY_LSHIFT, KEY_RIGHT))
		{
			mFramePos += PLAY_SPEED;
			clampFramePos();
		}
	}
	
	void logic()
	{
		handleMouseCmds();
		handleKeyboardCmds();
		
		mLastMousePos = mMouse.getPos();
		mMouse.update();
		mKeyboard.update();
		
		//printf("%f\n", float(bitmapMemUsage) / (sizeof(int) * test->w * test->h) * 100.f);
	}
	
	void draw(BITMAP* dest) const
	{
		int mx = int(mMouse.getPos().x);
		int my = int(mMouse.getPos().y);
		
		mWorkspaceVec[int(mFramePos)].draw(dest, 0, 0);
		if(mTool == TOOL_BRUSH)
			mBrush.draw(dest, mx, my);
		getCursor().draw(dest, mx, my);
	}
};

int main()
{
	bool quitFlag = false;
	
	init();
	
	Editor editor;
	
	try
	{
		editor.load("temp/Bust-A-Move (USA)_%05d.bmp", 6, 29);
	} 
	
	catch(std::exception const& e)
	{
		printf("%s", e.what());
		getch();
		exit(1);
	}
	
	while(!quitFlag)
	{
		if(key[KEY_ESC]) quitFlag = true;
		
		editor.logic();
		
		clear_to_color(buffer, 0);
		editor.draw(buffer);
		blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	}
	
	deinit();
	return 0;
}
END_OF_MAIN()
