/*
Class:Window
Author:Rich Davison
Description:Creates and handles the Window, including the initialisation of the mouse and keyboard.
*/
#pragma once

#include "common.h"
#include <string>

#include <windows.h>
#include <io.h>
#include <stdio.h>
#include <fcntl.h>

#include "OGLRenderer.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "GameTimer.h"



#define VC_EXTRALEAN
#define WINDOWCLASS "WindowClass"

class OGLRenderer;

class Window	{
public:
	/**
	* @brief 创建一个窗体
	* @param title 窗体的名称
	* @param sizeX 窗体的X大小
	* @param sizeY 窗体的Y大小
	* @param fullScreen 是否需要充满屏幕
	*/
	Window(std::string title = "OpenGL Framework", int sizeX = 800, int sizeY = 600, bool fullScreen = false);
	~Window(void);

	bool	UpdateWindow();	

	void	SetRenderer(OGLRenderer* r);

	HWND	GetHandle();
	/**
	* @brief 检查所有的构造器是否按照计划进行
	* @return 初始化状态
	*/
	bool	HasInitialised();

	void	LockMouseToWindow(bool lock);
	void	ShowOSPointer(bool show);

	const std::string& GetTitle()   const { return windowTitle; }
	void				SetTitle(const std::string& title) {
		windowTitle = title;
		SetWindowText(windowHandle, windowTitle.c_str());
	};

	Vector2	GetScreenSize() {return size;};

	static Keyboard*	GetKeyboard()	{return keyboard;}
	static Mouse*		GetMouse()		{return mouse;}

	GameTimer*   GetTimer()		{return timer;}

protected:
	void	CheckMessages(MSG &msg);
	static LRESULT CALLBACK WindowProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

	HWND			windowHandle;

	static Window*		window;
	static Keyboard*	keyboard;
	static Mouse*		mouse;

	GameTimer*	timer;

	OGLRenderer*		renderer;

	bool				forceQuit;
	bool				init;
	bool				fullScreen;//是否需要全屏
	bool				lockMouse;//是否需要锁定鼠标
	bool				showMouse;//是否需要显示鼠标
	bool				mouseLeftWindow;
	bool				isActive;//窗体是否被激活

	Vector2				position;
	Vector2				size;

	std::string			windowTitle;//窗口名称
};