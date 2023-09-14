// Template, IGAD version 3
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2023

#include "precomp.h"
#include "game.h"

#pragma comment( linker, "/subsystem:windows /ENTRY:mainCRTStartup" )

using namespace Tmpl8;

// Enable usage of dedicated GPUs in notebooks
// Note: this does cause the linker to produce a .lib and .exp file;
// see http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
// Alternatively, you can *disable* this to make your app use less power.
#ifdef WIN32
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

extern "C"
{
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

GLFWwindow* window = 0;
static bool hasFocus = true, running = true;
static GLTexture* renderTarget = 0;
static int scrwidth = 0, scrheight = 0;
static TheApp* app = 0;
bool IGP_detected = false;

// static member data for instruction set support class
static const CPUCaps cpucaps;

// provide access to the render target, for OpenCL / OpenGL interop
GLTexture* GetRenderTarget() { return renderTarget; }

// provide access to window focus state
bool WindowHasFocus() { return hasFocus; }

// GLFW callbacks
void InitRenderTarget(int w, int h) {
	// allocate render target and surface
	scrwidth = w, scrheight = h;
	renderTarget = new GLTexture(scrwidth, scrheight, GLTexture::INTTARGET);
}
void ReshapeWindowCallback(GLFWwindow*, int w, int h) {
	glViewport(0, 0, w, h);
}
void KeyEventCallback(GLFWwindow*, int key, int, int action, int) {
	if (key == GLFW_KEY_ESCAPE) running = false;
	if (app->input) if (key >= 0) app->input->KeyInput(key, action);
}

void CharEventCallback(GLFWwindow*, uint) { /* nothing here yet */ }
void WindowFocusCallback(GLFWwindow*, int focused) { hasFocus = (focused == GL_TRUE); }
void MouseButtonCallback(GLFWwindow*, int button, int action, int) {
	if (app->input) app->input->MouseInput(button, action);
}
void MouseScrollCallback(GLFWwindow*, double, double y) {
	app->input->MouseWheel((float) y);
}
void MousePosCallback(GLFWwindow*, double x, double y) {
	if (app->input) app->input->MouseMove((int) x, (int) y);
}
void ErrorCallback(int, const char* description) {
	fprintf(stderr, "GLFW Error: %s\n", description);
}

// Application entry point
void main() {
	// open a window
	if (!glfwInit()) FatalError("glfwInit failed.");
	glfwSetErrorCallback(ErrorCallback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 3.3 is enough for our needs
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_STENCIL_BITS, GL_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE /* easier :) */);
#ifdef FULLSCREEN
	window = glfwCreateWindow(SCRWIDTH, SCRHEIGHT, "Tmpl8-2023", glfwGetPrimaryMonitor(), 0);
#else
	window = glfwCreateWindow(SCRWIDTH, SCRHEIGHT, "Tmpl8-2023", 0, 0);
#endif
	if (!window) FatalError("glfwCreateWindow failed.");
	glfwMakeContextCurrent(window);
	// register callbacks
	glfwSetWindowSizeCallback(window, ReshapeWindowCallback);
	glfwSetKeyCallback(window, KeyEventCallback);
	glfwSetWindowFocusCallback(window, WindowFocusCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetScrollCallback(window, MouseScrollCallback);
	glfwSetCursorPosCallback(window, MousePosCallback);
	glfwSetCharCallback(window, CharEventCallback);
	// initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) FatalError("gladLoadGLLoader failed.");
	glfwSwapInterval(0);
	// prepare OpenGL state
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glViewport(0, 0, SCRWIDTH, SCRHEIGHT);
	CheckGL();
	char* vendor = (char*) glGetString(GL_VENDOR);
	char* renderer = (char*) glGetString(GL_RENDERER);
	if (strstr(vendor, "Intel")) IGP_detected = true;
	if (strstr(renderer, "Arc")) IGP_detected = false;
	// we want a console window for text output
#ifndef FULLSCREEN
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	AllocConsole();
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.X = 1280;
	coninfo.dwSize.Y = 800;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
	FILE* file = nullptr;
	freopen_s(&file, "CON", "w", stdout);
	freopen_s(&file, "CON", "w", stderr);
	SetWindowPos(GetConsoleWindow(), HWND_TOP, 0, 0, 1280, 800, 0);
	glfwShowWindow(window);
#endif
	// initialize application
	InitRenderTarget(SCRWIDTH, SCRHEIGHT);
	Surface* screen = new Surface(SCRWIDTH, SCRHEIGHT);
	Input* input = new Input();
	app = new Game();
	app->screen = screen;
	app->input = input;
	app->Init();
	// done, enter main loop
#if 0
	// crt shader, https://github.com/libretro/slang-shaders/tree/master/crt/shaders/hyllian
	char fs[] =
		"#version 330											\n"
		"uniform sampler2D c; in vec2 uv; out vec4 f;			\n"
		"#define SCRWIDTH             							\n"
		"#define SCRHEIGHT             							\n"
		"#define MASK_INTENSITY 0.5								\n"
		"#define InputGamma		2.4								\n"
		"#define OutputGamma	2.2								\n"
		"#define BRIGHTBOOST	1.5								\n"
		"#define SCANLINES		0.72							\n"
		"#define SHARPER		0								\n"
		"#define GAMMA_IN(color)  pow(color, vec3(InputGamma, InputGamma, InputGamma)) \n"
		"#define GAMMA_OUT(color) pow(color, vec3(1.0 / OutputGamma, 1.0 / OutputGamma, 1.0 / OutputGamma)) \n"
		"void main(){											\n"
		"vec2 vt = uv; /* vec2(uv.x-0.499999,uv.y); */			\n"
		"vec2 ss = vec2( SCRWIDTH, SCRHEIGHT );					\n"
		"vec2 ps = vec2( 1.0 / ss.x, 1.0 / ss.y );				\n"
		"vec2 dx = vec2( ps.x, 0.0 ), dy = vec2( 0.0, ps.y );	\n"
		"vec2 tc = (floor( vt.xy * ss.xy ) + vec2( 0.49999, 0.49999 )) / ss.xy; \n"
		"vec2 fp = fract( vt.xy * ss.xy );						\n"
		"vec3 c10 = texture( c, tc - dx ).xyz;					\n"
		"vec3 c11 = texture( c, tc ).xyz;						\n"
		"vec3 c12 = texture( c, tc + dx ).xyz;					\n"
		"vec3 c13 = texture( c, tc + 2.0 * dx ).xyz;			\n"
		"vec4 lobes = vec4( fp.x * fp.x * fp.x, fp.x * fp.x, fp.x, 1.0 ); \n"
		"vec4 i = vec4( 0.0 );									\n"
		"if (SHARPER == 0.0) { /* CATROM */						\n"
		"i.x = dot( vec4( -0.5, 1.0, -0.5, 0.0 ), lobes );		\n"
		"i.y = dot( vec4( 1.5, -2.5, 0.0, 1.0 ), lobes );		\n"
		"i.z = dot( vec4( -1.5, 2.0, 0.5, 0.0 ), lobes );		\n"
		"i.w = dot( vec4( 0.5, -0.5, 0.0, 0.0 ), lobes );		\n"
		"} else if (SHARPER == 1.0) { /* HERMITE */				\n"
		"i.x = dot( vec4( 0.0, 0.0, 0.0, 0.0 ), lobes );		\n"
		"i.y = dot( vec4( 2.0, -3.0, 0.0, 1.0 ), lobes );		\n"
		"i.z = dot( vec4( -2.0, 3.0, 0.0, 0.0 ), lobes );		\n"
		"i.w = dot( vec4( 0.0, 0.0, 0.0, 0.0 ), lobes );}		\n"
		"vec3 color = i.x * c10.xyz;							\n"
		"color += i.y * c11.xyz + i.z * c12.xyz + i.w * c13.xyz; \n"
		"color = GAMMA_IN( color );								\n"
		"float pos1 = 1.5 - SCANLINES - abs( fp.y - 0.5 );		\n"
		"float d1 = max( 0.0, min( 1.0, pos1 ) );				\n"
		"float d = d1 * d1 * (3.0 + BRIGHTBOOST - (2.0 * d1));	\n"
		"vec4 dmw = mix(										\n"
		"vec4( 1.0, 1.0 - MASK_INTENSITY, 1.0, 1. ),			\n"
		"vec4( 1.0 - MASK_INTENSITY, 1.0, 1.0 - MASK_INTENSITY, 1.),	\n"
		"floor( mod( vt.x * ss.x * 4, 2.0 ) ) );				\n"
		"color *= d * vec3( dmw.x, dmw.y, dmw.z );				\n"
		"f = vec4( GAMMA_OUT( color ), 1.0 );}";
	char* sw = strstr(fs, "SCRWIDTH "), * sh = strstr(fs, "SCRHEIGHT ");
	char swt[16], sht[16];
	sprintf(swt, "%i", SCRWIDTH / 4);
	sprintf(sht, "%i", SCRHEIGHT / 4);
	memcpy(sw + 9, swt, strlen(swt));
	memcpy(sh + 10, sht, strlen(sht));
	Shader* shader = new Shader(
		"#version 330\nin vec4 p;\nin vec2 t;out vec2 uv;void main(){uv=t;gl_Position=p;}",
		fs, true);
#else
#if 1
	// basic shader, no gamma correction
	Shader* shader = new Shader(
		"#version 330\nin vec4 p;\nin vec2 t;out vec2 u;void main(){u=t;gl_Position=p;}",
		"#version 330\nuniform sampler2D c;in vec2 u;out vec4 f;void main(){f=/*sqrt*/(texture(c,u));}", true);
#else
	// fxaa shader
	Shader* shader = new Shader(
		"#version 330\nin vec4 p;\nin vec2 t;out vec2 uv;void main(){uv=t;gl_Position=p;}",
		// FXAA 3.11 Implementation - effendiian
		// https://www.shadertoy.com/view/ttXGzn
		"#version 330\nuniform sampler2D tex;\nin vec2 uv;out vec4 f; \n"							\
		"#define FXAA_LUMINANCE 			1					\n"									\
		"#define FXAA_EDGE_THRESHOLD	  	(1.0/8.0)			\n"									\
		"#define FXAA_EDGE_THRESHOLD_MIN  	(1.0/24.0)			\n"									\
		"#define FXAA_SEARCH_STEPS			32					\n"									\
		"#define FXAA_SEARCH_ACCELERATION 	1					\n"									\
		"#define FXAA_SEARCH_THRESHOLD		(1.0/4.0)			\n"									\
		"#define FXAA_SUBPIX				2 // 1 is crisper	\n"									\
		"#define FXAA_SUBPIX_CAP			(3.0/4.0)			\n"									\
		"#define FXAA_SUBPIX_TRIM			(1.0/4.0)			\n"									\
		"#define FXAA_SUBPIX_TRIM_SCALE (1.0/(1.0 - FXAA_SUBPIX_TRIM))	\n"							\
		"float lum( vec3 color ) {\n #if FXAA_LUMINANCE == 0 \n"									\
		"	return color.x * 0.2126729 + color.y * 0.7151522 + color.z * 0.0721750; \n"				\
		"#else \n return color.g * (0.587 / 0.299) + color.r; \n #endif \n }"						\
		"float vertEdge( float lumaO, float lumaN, float lumaE, float lumaS,"						\
		"	float lumaW, float lumaNW, float lumaNE, float lumaSW, float lumaSE )"					\
		"{	float top = (0.25 * lumaNW) + (-0.5 * lumaN) + (0.25 * lumaNE);"						\
		"	float middle = (0.50 * lumaW) + (-1.0 * lumaO) + (0.50 * lumaE);"						\
		"	float bottom = (0.25 * lumaSW) + (-0.5 * lumaS) + (0.25 * lumaSE);"						\
		"	return abs( top ) + abs( middle ) + abs( bottom ); }"									\
		"float horEdge( float lumaO, float lumaN, float lumaE, float lumaS,"						\
		"	float lumaW, float lumaNW, float lumaNE, float lumaSW, float lumaSE )"					\
		"{	float top = (0.25 * lumaNW) + (-0.5 * lumaW) + (0.25 * lumaSW);"						\
		"	float middle = (0.50 * lumaN) + (-1.0 * lumaO) + (0.50 * lumaS);"						\
		"	float bottom = (0.25 * lumaNE) + (-0.5 * lumaE) + (0.25 * lumaSE);"						\
		"	return abs( top ) + abs( middle ) + abs( bottom ); }"									\
		"vec3 fxaa( vec2 textureDimensions, vec2 uv )"												\
		"{	vec2 texel = vec2( 1.0, 1.0 ) / textureDimensions;"										\
		"	vec3 rgbN = texture( tex, uv + vec2( 0, -texel.y ) ).rgb,"								\
		"		 rgbW = texture( tex, uv + vec2( -texel.x, 0 ) ).rgb,"								\
		"		 rgbO = texture( tex, uv + vec2( 0, 0 ) ).rgb,"										\
		"		 rgbE = texture( tex, uv + vec2( texel.x, 0 ) ).rgb,"								\
		"		 rgbS = texture( tex, uv + vec2( 0, texel.y ) ).rgb;"								\
		"	float lumaN = lum( rgbN ), lumaW = lum( rgbW );"										\
		"	float lumaO = lum( rgbO ), lumaE = lum( rgbE ), lumaS = lum( rgbS );"					\
		"	float minLuma = min( lumaO, min( min( lumaN, lumaW ), min( lumaS, lumaE ) ) );"			\
		"	float maxLuma = max( lumaO, max( max( lumaN, lumaW ), max( lumaS, lumaE ) ) );"			\
		"	float localContrast = maxLuma - minLuma;"												\
		"	if (localContrast < max( FXAA_EDGE_THRESHOLD_MIN, maxLuma* FXAA_EDGE_THRESHOLD ))"		\
		"		return rgbO;"																		\
		"	vec3 rgbL = rgbN + rgbW + rgbO + rgbE + rgbS;"											\
		"	float lumaL = (lumaN + lumaW + lumaS + lumaE) * 0.25;"									\
		"	float pixelContrast = abs( lumaL - lumaO );"											\
		"	float contrastRatio = pixelContrast / localContrast;"									\
		"	float lowpassBlend = 0;			\n"														\
		"#if FXAA_SUBPIX == 1				\n"														\
		"	lowpassBlend = max( 0.0, contrastRatio - FXAA_SUBPIX_TRIM ) * FXAA_SUBPIX_TRIM_SCALE;"	\
		"	lowpassBlend = min( FXAA_SUBPIX_CAP, lowpassBlend );	\n"								\
		"#elif FXAA_SUBPIX == 2				\n"														\
		"	lowpassBlend = contrastRatio;	\n"														\
		"#endif								\n"														\
		"	vec3 rgbNW = texture( tex, uv + vec2( -texel.x, -texel.y ) ).rgb,"						\
		"		 rgbNE = texture( tex, uv + vec2( texel.x, -texel.y ) ).rgb,"						\
		"		 rgbSW = texture( tex, uv + vec2( -texel.x, texel.y ) ).rgb,"						\
		"		 rgbSE = texture( tex, uv + vec2( texel.x, texel.y ) ).rgb;"						\
		"	rgbL += (rgbNW + rgbNE + rgbSW + rgbSE);"												\
		"	rgbL *= (1.0 / 9.0);"																	\
		"	float lumaNW = lum( rgbNW ), lumaNE = lum( rgbNE );"									\
		"	float lumaSW = lum( rgbSW ), lumaSE = lum( rgbSE );"									\
		"	float edgeVert = vertEdge( lumaO, lumaN, lumaE, lumaS, lumaW, lumaNW, lumaNE, lumaSW, lumaSE );" \
		"	float edgeHori = horEdge( lumaO, lumaN, lumaE, lumaS, lumaW, lumaNW, lumaNE, lumaSW, lumaSE );" \
		"	bool isHorizontal = edgeHori >= edgeVert;"												\
		"	float edgeSign = isHorizontal ? -texel.y : -texel.x;"									\
		"	float gradNeg = isHorizontal ? abs( lumaN - lumaO ) : abs( lumaW - lumaO );"			\
		"	float gradPos = isHorizontal ? abs( lumaS - lumaO ) : abs( lumaE - lumaO );"			\
		"	float lumaNeg = isHorizontal ? ((lumaN + lumaO) * 0.5) : ((lumaW + lumaO) * 0.5);"		\
		"	float lumaPos = isHorizontal ? ((lumaS + lumaO) * 0.5) : ((lumaE + lumaO) * 0.5);"		\
		"	bool isNegative = (gradNeg >= gradPos);"												\
		"	float gradientHighest = isNegative ? gradNeg : gradPos;"								\
		"	float lumaHighest = isNegative ? lumaNeg : lumaPos;"									\
		"	if (isNegative) edgeSign *= -1.0;"														\
		"	vec2 pointN = vec2( 0.0, 0.0 );"														\
		"	pointN.x = uv.x + (isHorizontal ? 0.0 : edgeSign * 0.5);"								\
		"	pointN.y = uv.y + (isHorizontal ? edgeSign * 0.5 : 0.0);"								\
		"	gradientHighest *= FXAA_SEARCH_THRESHOLD;"												\
		"	vec2 pointP = pointN;"																	\
		"	vec2 offset = isHorizontal ? vec2( texel.x, 0.0 ) : vec2( 0.0, texel.y );"				\
		"	float lumaNegEnd = lumaNeg, lumaPosEnd = lumaPos;"										\
		"	bool searchNeg = false, searchPos = false;\n"											\
		"#if FXAA_SEARCH_ACCELERATION == 1\n"														\
		"	pointN -= offset, pointP += offset;\n"													\
		"#elif FXAA_SEARCH_ACCELERATION == 2\n"														\
		"	pointN -= offset * 1.5, pointP += offset * 1.5, offset *= 2;\n"							\
		"#elif FXAA_SEARCH_ACCELERATION == 3\n"														\
		"	pointN -= offset * 2, pointP += offset * 2, offset *= 3;\n"								\
		"#elif FXAA_SEARCH_ACCELERATION == 4\n"														\
		"	pointN -= offset * 2.5, pointP += offset * 2.5, offset *= 4;\n"							\
		"#endif\n"																					\
		"	for (int i = 0; i < FXAA_SEARCH_STEPS; i++) {\n"										\
		"	#if FXAA_SEARCH_ACCELERATION == 1\n"													\
		"		if (!searchNeg) lumaNegEnd = lum( texture( tex, pointN ).rgb );"					\
		"		if (!searchPos) lumaPosEnd = lum( texture( tex, pointP ).rgb );\n"					\
		"	#else \n"																				\
		"		if (!searchNeg) lumaNegEnd = lum( textureGrad( tex, pointN, offset, offset ).rgb );"	\
		"		if (!searchPos) lumaPosEnd = lum( textureGrad( tex, pointP, offset, offset ).rgb );\n"	\
		"	#endif \n"																				\
		"		searchNeg = searchNeg || (abs( lumaNegEnd - lumaHighest ) >= gradientHighest);"		\
		"		searchPos = searchPos || (abs( lumaPosEnd - lumaPos ) >= gradPos);"					\
		"		if (searchNeg && searchPos) break;"													\
		"		if (!searchNeg) pointN -= offset;"													\
		"		if (!searchPos) pointP += offset; }"												\
		"	float distanceNeg = isHorizontal ? uv.x - pointN.x : uv.y - pointN.y;"					\
		"	float distancePos = isHorizontal ? pointP.x - uv.x : pointP.y - uv.y;"					\
		"	bool isCloserToNegative = distanceNeg < distancePos;"									\
		"	float lumaEnd = isCloserToNegative ? lumaNegEnd : lumaPosEnd;"							\
		"	if (((lumaO - lumaNeg) < 0.0) == ((lumaEnd - lumaNeg) < 0.0)) edgeSign = 0.0;"			\
		"	float spanLen = distancePos + distanceNeg;"												\
		"	float dist = isCloserToNegative ? distanceNeg : distancePos;"							\
		"	float subOffs = (0.5 + (dist * (-1.0 / spanLen))) * edgeSign;"							\
		"	vec3 rgbOffset = textureLod( tex, vec2( uv.x + (isHorizontal ? 0.0 :"					\
		"		subOffs), uv.y + (isHorizontal ? subOffs : 0.0) ), 0.0 ).rgb;"						\
		"	return mix( rgbOffset, rgbL, lowpassBlend ); }"											\
		"void main(){f=vec4(sqrt(fxaa(vec2(1240,800),uv)),1);}", true);
#endif
#endif
	float deltaTime = 0;
	static int frameNr = 0;
	static Timer timer;
	while (!glfwWindowShouldClose(window)) {
		deltaTime = min(500.0f, 1000.0f * timer.elapsed());
		timer.reset();
		app->Tick(deltaTime);
		app->input->UpdateInputState();
		// send the rendering result to the screen using OpenGL
		if (frameNr++ > 1) {
			if (app->screen) renderTarget->CopyFrom(app->screen);
			shader->Bind();
			shader->SetInputTexture(0, "c", renderTarget);
			DrawQuad();
			shader->Unbind();
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		if (!running) break;
	}
	// close down
	app->Shutdown();
	Kernel::KillCL();
	glfwDestroyWindow(window);
	glfwTerminate();
}

// Jobmanager implementation
DWORD JobThreadProc(LPVOID lpParameter) {
	JobThread* JobThreadInstance = (JobThread*) lpParameter;
	if (JobThreadInstance /* just here to avoid 'unreachable code' warning */)
		JobThreadInstance->BackgroundTask();
	return 0;
}

void JobThread::CreateAndStartThread(unsigned int threadId) {
	m_GoSignal = CreateEvent(0, FALSE, FALSE, 0);
	m_ThreadHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE) &JobThreadProc, (LPVOID) this, 0, 0);
	m_ThreadID = threadId;
}
void JobThread::BackgroundTask() {
	while (1) {
		WaitForSingleObject(m_GoSignal, INFINITE);
		while (1) {
			Job* job = JobManager::GetJobManager()->GetNextJob();
			if (!job) {
				JobManager::GetJobManager()->ThreadDone(m_ThreadID);
				break;
			}
			job->RunCodeWrapper();
		}
	}
}

void JobThread::Go() {
	SetEvent(m_GoSignal);
}

void Job::RunCodeWrapper() {
	Main();
}

JobManager* JobManager::m_JobManager = 0;

JobManager::JobManager(unsigned int threads) : m_NumThreads(threads) {
	InitializeCriticalSection(&m_CS);
}

JobManager::~JobManager() {
	DeleteCriticalSection(&m_CS);
}

void JobManager::CreateJobManager(unsigned int numThreads) {
	m_JobManager = new JobManager(numThreads);
	m_JobManager->m_JobThreadList = new JobThread[numThreads];
	for (unsigned int i = 0; i < numThreads; i++) {
		m_JobManager->m_JobThreadList[i].CreateAndStartThread(i);
		m_JobManager->m_ThreadDone[i] = CreateEvent(0, FALSE, FALSE, 0);
	}
	m_JobManager->m_JobCount = 0;
}

void JobManager::AddJob2(Job* a_Job) {
	m_JobList[m_JobCount++] = a_Job;
}

Job* JobManager::GetNextJob() {
	Job* job = 0;
	EnterCriticalSection(&m_CS);
	if (m_JobCount > 0) job = m_JobList[--m_JobCount];
	LeaveCriticalSection(&m_CS);
	return job;
}

void JobManager::RunJobs() {
	if (m_JobCount == 0) return;
	for (unsigned int i = 0; i < m_NumThreads; i++) m_JobThreadList[i].Go();
	WaitForMultipleObjects(m_NumThreads, m_ThreadDone, TRUE, INFINITE);
}

void JobManager::ThreadDone(unsigned int n) {
	SetEvent(m_ThreadDone[n]);
}

DWORD CountSetBits(ULONG_PTR bitMask) {
	DWORD LSHIFT = sizeof(ULONG_PTR) * 8 - 1, bitSetCount = 0;
	ULONG_PTR bitTest = (ULONG_PTR) 1 << LSHIFT;
	for (DWORD i = 0; i <= LSHIFT; ++i) bitSetCount += ((bitMask & bitTest) ? 1 : 0), bitTest /= 2;
	return bitSetCount;
}

void JobManager::GetProcessorCount(uint& cores, uint& logical) {
	// https://github.com/GPUOpen-LibrariesAndSDKs/cpu-core-counts
	cores = logical = 0;
	char* buffer = NULL;
	DWORD len = 0;
	if (FALSE == GetLogicalProcessorInformationEx(RelationAll, (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX) buffer, &len)) {
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
			buffer = (char*) malloc(len);
			if (GetLogicalProcessorInformationEx(RelationAll, (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX) buffer, &len)) {
				char* ptr = buffer;
				while (ptr < buffer + len) {
					PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pi = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX) ptr;
					if (pi->Relationship == RelationProcessorCore) {
						cores++;
						for (size_t g = 0; g < pi->Processor.GroupCount; ++g)
							logical += CountSetBits(pi->Processor.GroupMask[g].Mask);
					}
					ptr += pi->Size;
				}
			}
			free(buffer);
		}
	}
}

JobManager* JobManager::GetJobManager() {
	if (!m_JobManager) {
		uint c, l;
		GetProcessorCount(c, l);
		CreateJobManager(l);
	}
	return m_JobManager;
}

// Helper functions
bool FileIsNewer(const char* file1, const char* file2) {
	struct stat f1;
	struct stat f2;

	auto ret = stat(file1, &f1);
	FATALERROR_IF(ret, "File %s not found!", file1);

	if (stat(file2, &f2)) return true; // second file does not exist

#ifdef _MSC_VER
	return f1.st_mtime >= f2.st_mtime;
#else
	if (f1.st_mtim.tv_sec >= f2.st_mtim.tv_sec)
		return true;
	return f1.st_mtim.tv_nsec >= f2.st_mtim.tv_nsec;
#endif
}

bool FileExists(const char* f) {
	ifstream s(f);
	return s.good();
}

bool RemoveFile(const char* f) {
	if (!FileExists(f)) return false;
	return !remove(f);
}

uint FileSize(string filename) {
	ifstream s(filename);
	return s.good();
}

string TextFileRead(const char* _File) {
	ifstream s(_File);
	string str((istreambuf_iterator<char>(s)), istreambuf_iterator<char>());
	s.close();
	return str;
}

int LineCount(const string s) {
	const char* p = s.c_str();
	int lines = 0;
	while (*p) if (*p++ == '\n') lines++;
	return lines;
}

void TextFileWrite(const string& text, const char* _File) {
	ofstream s(_File, ios::binary);
	int len = (int) text.size();
	s.write((const char*) &len, sizeof(len));
	s.write(text.c_str(), len);
}

/*

	OpenGL loader generated by glad 0.1.35 on Fri Mar 18 11:02:23 2022.

	Language/Generator: C/C++
	Specification: gl
	APIs: gl=3.3
	Profile: core
	Extensions:

	Loader: True
	Local files: True
	Omit khrplatform: False
	Reproducible: False

	Commandline:
		--profile="core" --api="gl=3.3" --generator="c" --spec="gl" --local-files --extensions=""
	Online:
		https://glad.dav1d.de/#profile=core&language=c&specification=gl&loader=on&api=gl%3D3.3
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glad.h"

static void* get_proc(const char* namez);

#if defined(_WIN32) || defined(__CYGWIN__)
#ifndef _WINDOWS_
#undef APIENTRY
#endif
#include <windows.h>
static HMODULE libGL;

typedef void* (APIENTRYP PFNWGLGETPROCADDRESSPROC_PRIVATE)(const char*);
static PFNWGLGETPROCADDRESSPROC_PRIVATE gladGetProcAddressPtr;

#ifdef _MSC_VER
#ifdef __has_include
#if __has_include(<winapifamily.h>)
#define HAVE_WINAPIFAMILY 1
#endif
#elif _MSC_VER >= 1700 && !_USING_V110_SDK71_
#define HAVE_WINAPIFAMILY 1
#endif
#endif

#ifdef HAVE_WINAPIFAMILY
#include <winapifamily.h>
#if !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP) && WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
#define IS_UWP 1
#endif
#endif

static
int open_gl(void) {
#ifndef IS_UWP
	libGL = LoadLibraryW(L"opengl32.dll");
	if (libGL != NULL) {
		void (*tmp)(void);
		tmp = (void(*)(void)) GetProcAddress(libGL, "wglGetProcAddress");
		gladGetProcAddressPtr = (PFNWGLGETPROCADDRESSPROC_PRIVATE) tmp;
		return gladGetProcAddressPtr != NULL;
	}
#endif

	return 0;
}

static
void close_gl(void) {
	if (libGL != NULL) {
		FreeLibrary((HMODULE) libGL);
		libGL = NULL;
	}
}
#else
#include <dlfcn.h>
static void* libGL;

#if !defined(__APPLE__) && !defined(__HAIKU__)
typedef void* (APIENTRYP PFNGLXGETPROCADDRESSPROC_PRIVATE)(const char*);
static PFNGLXGETPROCADDRESSPROC_PRIVATE gladGetProcAddressPtr;
#endif

static
int open_gl(void) {
#ifdef __APPLE__
	static const char* NAMES[] = {
		"../Frameworks/OpenGL.framework/OpenGL",
		"/Library/Frameworks/OpenGL.framework/OpenGL",
		"/System/Library/Frameworks/OpenGL.framework/OpenGL",
		"/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL"
	};
#else
	static const char* NAMES[] = {"libGL.so.1", "libGL.so"};
#endif

	unsigned int index = 0;
	for (index = 0; index < (sizeof(NAMES) / sizeof(NAMES[0])); index++) {
		libGL = dlopen(NAMES[index], RTLD_NOW | RTLD_GLOBAL);

		if (libGL != NULL) {
#if defined(__APPLE__) || defined(__HAIKU__)
			return 1;
#else
			gladGetProcAddressPtr = (PFNGLXGETPROCADDRESSPROC_PRIVATE) dlsym(libGL,
				"glXGetProcAddressARB");
			return gladGetProcAddressPtr != NULL;
#endif
		}
	}

	return 0;
}

static
void close_gl(void) {
	if (libGL != NULL) {
		dlclose(libGL);
		libGL = NULL;
	}
}
#endif

static
void* get_proc(const char* namez) {
	void* result = NULL;
	if (libGL == NULL) return NULL;

#if !defined(__APPLE__) && !defined(__HAIKU__)
	if (gladGetProcAddressPtr != NULL) {
		result = gladGetProcAddressPtr(namez);
	}
#endif
	if (result == NULL) {
#if defined(_WIN32) || defined(__CYGWIN__)
		result = (void*) GetProcAddress((HMODULE) libGL, namez);
#else
		result = dlsym(libGL, namez);
#endif
	}

	return result;
}

int gladLoadGL(void) {
	int status = 0;

	if (open_gl()) {
		status = gladLoadGLLoader(&get_proc);
		close_gl();
	}

	return status;
}

struct gladGLversionStruct GLVersion = {0, 0};

#if defined(GL_ES_VERSION_3_0) || defined(GL_VERSION_3_0)
#define _GLAD_IS_SOME_NEW_VERSION 1
#endif

static int max_loaded_major;
static int max_loaded_minor;

static const char* exts = NULL;
static int num_exts_i = 0;
static char** exts_i = NULL;

static int get_exts(void) {
#ifdef _GLAD_IS_SOME_NEW_VERSION
	if (max_loaded_major < 3) {
#endif
		exts = (const char*) glGetString(GL_EXTENSIONS);
#ifdef _GLAD_IS_SOME_NEW_VERSION
	}
	else {
		unsigned int index;

		num_exts_i = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &num_exts_i);
		if (num_exts_i > 0) {
			exts_i = (char**) malloc((size_t) num_exts_i * (sizeof * exts_i));
		}

		if (exts_i == NULL) {
			return 0;
		}

		for (index = 0; index < (unsigned) num_exts_i; index++) {
			const char* gl_str_tmp = (const char*) glGetStringi(GL_EXTENSIONS, index);
			size_t len = strlen(gl_str_tmp);

			char* local_str = (char*) malloc((len + 1) * sizeof(char));
			if (local_str != NULL) {
				memcpy(local_str, gl_str_tmp, (len + 1) * sizeof(char));
			}
			exts_i[index] = local_str;
		}
	}
#endif
	return 1;
}

static void free_exts(void) {
	if (exts_i != NULL) {
		int index;
		for (index = 0; index < num_exts_i; index++) {
			free((char*) exts_i[index]);
		}
		free((void*) exts_i);
		exts_i = NULL;
	}
}

static int has_ext(const char* ext) {
#ifdef _GLAD_IS_SOME_NEW_VERSION
	if (max_loaded_major < 3) {
#endif
		const char* extensions;
		const char* loc;
		const char* terminator;
		extensions = exts;
		if (extensions == NULL || ext == NULL) {
			return 0;
		}

		while (1) {
			loc = strstr(extensions, ext);
			if (loc == NULL) {
				return 0;
			}

			terminator = loc + strlen(ext);
			if ((loc == extensions || *(loc - 1) == ' ') &&
				(*terminator == ' ' || *terminator == '\0')) {
				return 1;
			}
			extensions = terminator;
		}
#ifdef _GLAD_IS_SOME_NEW_VERSION
	}
	else {
		int index;
		if (exts_i == NULL) return 0;
		for (index = 0; index < num_exts_i; index++) {
			const char* e = exts_i[index];

			if (exts_i[index] != NULL && strcmp(e, ext) == 0) {
				return 1;
			}
		}
	}
#endif

	return 0;
}
int GLAD_GL_VERSION_1_0 = 0;
int GLAD_GL_VERSION_1_1 = 0;
int GLAD_GL_VERSION_1_2 = 0;
int GLAD_GL_VERSION_1_3 = 0;
int GLAD_GL_VERSION_1_4 = 0;
int GLAD_GL_VERSION_1_5 = 0;
int GLAD_GL_VERSION_2_0 = 0;
int GLAD_GL_VERSION_2_1 = 0;
int GLAD_GL_VERSION_3_0 = 0;
int GLAD_GL_VERSION_3_1 = 0;
int GLAD_GL_VERSION_3_2 = 0;
int GLAD_GL_VERSION_3_3 = 0;
PFNGLACTIVETEXTUREPROC glad_glActiveTexture = NULL;
PFNGLATTACHSHADERPROC glad_glAttachShader = NULL;
PFNGLBEGINCONDITIONALRENDERPROC glad_glBeginConditionalRender = NULL;
PFNGLBEGINQUERYPROC glad_glBeginQuery = NULL;
PFNGLBEGINTRANSFORMFEEDBACKPROC glad_glBeginTransformFeedback = NULL;
PFNGLBINDATTRIBLOCATIONPROC glad_glBindAttribLocation = NULL;
PFNGLBINDBUFFERPROC glad_glBindBuffer = NULL;
PFNGLBINDBUFFERBASEPROC glad_glBindBufferBase = NULL;
PFNGLBINDBUFFERRANGEPROC glad_glBindBufferRange = NULL;
PFNGLBINDFRAGDATALOCATIONPROC glad_glBindFragDataLocation = NULL;
PFNGLBINDFRAGDATALOCATIONINDEXEDPROC glad_glBindFragDataLocationIndexed = NULL;
PFNGLBINDFRAMEBUFFERPROC glad_glBindFramebuffer = NULL;
PFNGLBINDRENDERBUFFERPROC glad_glBindRenderbuffer = NULL;
PFNGLBINDSAMPLERPROC glad_glBindSampler = NULL;
PFNGLBINDTEXTUREPROC glad_glBindTexture = NULL;
PFNGLBINDVERTEXARRAYPROC glad_glBindVertexArray = NULL;
PFNGLBLENDCOLORPROC glad_glBlendColor = NULL;
PFNGLBLENDEQUATIONPROC glad_glBlendEquation = NULL;
PFNGLBLENDEQUATIONSEPARATEPROC glad_glBlendEquationSeparate = NULL;
PFNGLBLENDFUNCPROC glad_glBlendFunc = NULL;
PFNGLBLENDFUNCSEPARATEPROC glad_glBlendFuncSeparate = NULL;
PFNGLBLITFRAMEBUFFERPROC glad_glBlitFramebuffer = NULL;
PFNGLBUFFERDATAPROC glad_glBufferData = NULL;
PFNGLBUFFERSUBDATAPROC glad_glBufferSubData = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glad_glCheckFramebufferStatus = NULL;
PFNGLCLAMPCOLORPROC glad_glClampColor = NULL;
PFNGLCLEARPROC glad_glClear = NULL;
PFNGLCLEARBUFFERFIPROC glad_glClearBufferfi = NULL;
PFNGLCLEARBUFFERFVPROC glad_glClearBufferfv = NULL;
PFNGLCLEARBUFFERIVPROC glad_glClearBufferiv = NULL;
PFNGLCLEARBUFFERUIVPROC glad_glClearBufferuiv = NULL;
PFNGLCLEARCOLORPROC glad_glClearColor = NULL;
PFNGLCLEARDEPTHPROC glad_glClearDepth = NULL;
PFNGLCLEARSTENCILPROC glad_glClearStencil = NULL;
PFNGLCLIENTWAITSYNCPROC glad_glClientWaitSync = NULL;
PFNGLCOLORMASKPROC glad_glColorMask = NULL;
PFNGLCOLORMASKIPROC glad_glColorMaski = NULL;
PFNGLCOLORP3UIPROC glad_glColorP3ui = NULL;
PFNGLCOLORP3UIVPROC glad_glColorP3uiv = NULL;
PFNGLCOLORP4UIPROC glad_glColorP4ui = NULL;
PFNGLCOLORP4UIVPROC glad_glColorP4uiv = NULL;
PFNGLCOMPILESHADERPROC glad_glCompileShader = NULL;
PFNGLCOMPRESSEDTEXIMAGE1DPROC glad_glCompressedTexImage1D = NULL;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glad_glCompressedTexImage2D = NULL;
PFNGLCOMPRESSEDTEXIMAGE3DPROC glad_glCompressedTexImage3D = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glad_glCompressedTexSubImage1D = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glad_glCompressedTexSubImage2D = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glad_glCompressedTexSubImage3D = NULL;
PFNGLCOPYBUFFERSUBDATAPROC glad_glCopyBufferSubData = NULL;
PFNGLCOPYTEXIMAGE1DPROC glad_glCopyTexImage1D = NULL;
PFNGLCOPYTEXIMAGE2DPROC glad_glCopyTexImage2D = NULL;
PFNGLCOPYTEXSUBIMAGE1DPROC glad_glCopyTexSubImage1D = NULL;
PFNGLCOPYTEXSUBIMAGE2DPROC glad_glCopyTexSubImage2D = NULL;
PFNGLCOPYTEXSUBIMAGE3DPROC glad_glCopyTexSubImage3D = NULL;
PFNGLCREATEPROGRAMPROC glad_glCreateProgram = NULL;
PFNGLCREATESHADERPROC glad_glCreateShader = NULL;
PFNGLCULLFACEPROC glad_glCullFace = NULL;
PFNGLDELETEBUFFERSPROC glad_glDeleteBuffers = NULL;
PFNGLDELETEFRAMEBUFFERSPROC glad_glDeleteFramebuffers = NULL;
PFNGLDELETEPROGRAMPROC glad_glDeleteProgram = NULL;
PFNGLDELETEQUERIESPROC glad_glDeleteQueries = NULL;
PFNGLDELETERENDERBUFFERSPROC glad_glDeleteRenderbuffers = NULL;
PFNGLDELETESAMPLERSPROC glad_glDeleteSamplers = NULL;
PFNGLDELETESHADERPROC glad_glDeleteShader = NULL;
PFNGLDELETESYNCPROC glad_glDeleteSync = NULL;
PFNGLDELETETEXTURESPROC glad_glDeleteTextures = NULL;
PFNGLDELETEVERTEXARRAYSPROC glad_glDeleteVertexArrays = NULL;
PFNGLDEPTHFUNCPROC glad_glDepthFunc = NULL;
PFNGLDEPTHMASKPROC glad_glDepthMask = NULL;
PFNGLDEPTHRANGEPROC glad_glDepthRange = NULL;
PFNGLDETACHSHADERPROC glad_glDetachShader = NULL;
PFNGLDISABLEPROC glad_glDisable = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glad_glDisableVertexAttribArray = NULL;
PFNGLDISABLEIPROC glad_glDisablei = NULL;
PFNGLDRAWARRAYSPROC glad_glDrawArrays = NULL;
PFNGLDRAWARRAYSINSTANCEDPROC glad_glDrawArraysInstanced = NULL;
PFNGLDRAWBUFFERPROC glad_glDrawBuffer = NULL;
PFNGLDRAWBUFFERSPROC glad_glDrawBuffers = NULL;
PFNGLDRAWELEMENTSPROC glad_glDrawElements = NULL;
PFNGLDRAWELEMENTSBASEVERTEXPROC glad_glDrawElementsBaseVertex = NULL;
PFNGLDRAWELEMENTSINSTANCEDPROC glad_glDrawElementsInstanced = NULL;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glad_glDrawElementsInstancedBaseVertex = NULL;
PFNGLDRAWRANGEELEMENTSPROC glad_glDrawRangeElements = NULL;
PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glad_glDrawRangeElementsBaseVertex = NULL;
PFNGLENABLEPROC glad_glEnable = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glad_glEnableVertexAttribArray = NULL;
PFNGLENABLEIPROC glad_glEnablei = NULL;
PFNGLENDCONDITIONALRENDERPROC glad_glEndConditionalRender = NULL;
PFNGLENDQUERYPROC glad_glEndQuery = NULL;
PFNGLENDTRANSFORMFEEDBACKPROC glad_glEndTransformFeedback = NULL;
PFNGLFENCESYNCPROC glad_glFenceSync = NULL;
PFNGLFINISHPROC glad_glFinish = NULL;
PFNGLFLUSHPROC glad_glFlush = NULL;
PFNGLFLUSHMAPPEDBUFFERRANGEPROC glad_glFlushMappedBufferRange = NULL;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glad_glFramebufferRenderbuffer = NULL;
PFNGLFRAMEBUFFERTEXTUREPROC glad_glFramebufferTexture = NULL;
PFNGLFRAMEBUFFERTEXTURE1DPROC glad_glFramebufferTexture1D = NULL;
PFNGLFRAMEBUFFERTEXTURE2DPROC glad_glFramebufferTexture2D = NULL;
PFNGLFRAMEBUFFERTEXTURE3DPROC glad_glFramebufferTexture3D = NULL;
PFNGLFRAMEBUFFERTEXTURELAYERPROC glad_glFramebufferTextureLayer = NULL;
PFNGLFRONTFACEPROC glad_glFrontFace = NULL;
PFNGLGENBUFFERSPROC glad_glGenBuffers = NULL;
PFNGLGENFRAMEBUFFERSPROC glad_glGenFramebuffers = NULL;
PFNGLGENQUERIESPROC glad_glGenQueries = NULL;
PFNGLGENRENDERBUFFERSPROC glad_glGenRenderbuffers = NULL;
PFNGLGENSAMPLERSPROC glad_glGenSamplers = NULL;
PFNGLGENTEXTURESPROC glad_glGenTextures = NULL;
PFNGLGENVERTEXARRAYSPROC glad_glGenVertexArrays = NULL;
PFNGLGENERATEMIPMAPPROC glad_glGenerateMipmap = NULL;
PFNGLGETACTIVEATTRIBPROC glad_glGetActiveAttrib = NULL;
PFNGLGETACTIVEUNIFORMPROC glad_glGetActiveUniform = NULL;
PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glad_glGetActiveUniformBlockName = NULL;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC glad_glGetActiveUniformBlockiv = NULL;
PFNGLGETACTIVEUNIFORMNAMEPROC glad_glGetActiveUniformName = NULL;
PFNGLGETACTIVEUNIFORMSIVPROC glad_glGetActiveUniformsiv = NULL;
PFNGLGETATTACHEDSHADERSPROC glad_glGetAttachedShaders = NULL;
PFNGLGETATTRIBLOCATIONPROC glad_glGetAttribLocation = NULL;
PFNGLGETBOOLEANI_VPROC glad_glGetBooleani_v = NULL;
PFNGLGETBOOLEANVPROC glad_glGetBooleanv = NULL;
PFNGLGETBUFFERPARAMETERI64VPROC glad_glGetBufferParameteri64v = NULL;
PFNGLGETBUFFERPARAMETERIVPROC glad_glGetBufferParameteriv = NULL;
PFNGLGETBUFFERPOINTERVPROC glad_glGetBufferPointerv = NULL;
PFNGLGETBUFFERSUBDATAPROC glad_glGetBufferSubData = NULL;
PFNGLGETCOMPRESSEDTEXIMAGEPROC glad_glGetCompressedTexImage = NULL;
PFNGLGETDOUBLEVPROC glad_glGetDoublev = NULL;
PFNGLGETERRORPROC glad_glGetError = NULL;
PFNGLGETFLOATVPROC glad_glGetFloatv = NULL;
PFNGLGETFRAGDATAINDEXPROC glad_glGetFragDataIndex = NULL;
PFNGLGETFRAGDATALOCATIONPROC glad_glGetFragDataLocation = NULL;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glad_glGetFramebufferAttachmentParameteriv = NULL;
PFNGLGETINTEGER64I_VPROC glad_glGetInteger64i_v = NULL;
PFNGLGETINTEGER64VPROC glad_glGetInteger64v = NULL;
PFNGLGETINTEGERI_VPROC glad_glGetIntegeri_v = NULL;
PFNGLGETINTEGERVPROC glad_glGetIntegerv = NULL;
PFNGLGETMULTISAMPLEFVPROC glad_glGetMultisamplefv = NULL;
PFNGLGETPROGRAMINFOLOGPROC glad_glGetProgramInfoLog = NULL;
PFNGLGETPROGRAMIVPROC glad_glGetProgramiv = NULL;
PFNGLGETQUERYOBJECTI64VPROC glad_glGetQueryObjecti64v = NULL;
PFNGLGETQUERYOBJECTIVPROC glad_glGetQueryObjectiv = NULL;
PFNGLGETQUERYOBJECTUI64VPROC glad_glGetQueryObjectui64v = NULL;
PFNGLGETQUERYOBJECTUIVPROC glad_glGetQueryObjectuiv = NULL;
PFNGLGETQUERYIVPROC glad_glGetQueryiv = NULL;
PFNGLGETRENDERBUFFERPARAMETERIVPROC glad_glGetRenderbufferParameteriv = NULL;
PFNGLGETSAMPLERPARAMETERIIVPROC glad_glGetSamplerParameterIiv = NULL;
PFNGLGETSAMPLERPARAMETERIUIVPROC glad_glGetSamplerParameterIuiv = NULL;
PFNGLGETSAMPLERPARAMETERFVPROC glad_glGetSamplerParameterfv = NULL;
PFNGLGETSAMPLERPARAMETERIVPROC glad_glGetSamplerParameteriv = NULL;
PFNGLGETSHADERINFOLOGPROC glad_glGetShaderInfoLog = NULL;
PFNGLGETSHADERSOURCEPROC glad_glGetShaderSource = NULL;
PFNGLGETSHADERIVPROC glad_glGetShaderiv = NULL;
PFNGLGETSTRINGPROC glad_glGetString = NULL;
PFNGLGETSTRINGIPROC glad_glGetStringi = NULL;
PFNGLGETSYNCIVPROC glad_glGetSynciv = NULL;
PFNGLGETTEXIMAGEPROC glad_glGetTexImage = NULL;
PFNGLGETTEXLEVELPARAMETERFVPROC glad_glGetTexLevelParameterfv = NULL;
PFNGLGETTEXLEVELPARAMETERIVPROC glad_glGetTexLevelParameteriv = NULL;
PFNGLGETTEXPARAMETERIIVPROC glad_glGetTexParameterIiv = NULL;
PFNGLGETTEXPARAMETERIUIVPROC glad_glGetTexParameterIuiv = NULL;
PFNGLGETTEXPARAMETERFVPROC glad_glGetTexParameterfv = NULL;
PFNGLGETTEXPARAMETERIVPROC glad_glGetTexParameteriv = NULL;
PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glad_glGetTransformFeedbackVarying = NULL;
PFNGLGETUNIFORMBLOCKINDEXPROC glad_glGetUniformBlockIndex = NULL;
PFNGLGETUNIFORMINDICESPROC glad_glGetUniformIndices = NULL;
PFNGLGETUNIFORMLOCATIONPROC glad_glGetUniformLocation = NULL;
PFNGLGETUNIFORMFVPROC glad_glGetUniformfv = NULL;
PFNGLGETUNIFORMIVPROC glad_glGetUniformiv = NULL;
PFNGLGETUNIFORMUIVPROC glad_glGetUniformuiv = NULL;
PFNGLGETVERTEXATTRIBIIVPROC glad_glGetVertexAttribIiv = NULL;
PFNGLGETVERTEXATTRIBIUIVPROC glad_glGetVertexAttribIuiv = NULL;
PFNGLGETVERTEXATTRIBPOINTERVPROC glad_glGetVertexAttribPointerv = NULL;
PFNGLGETVERTEXATTRIBDVPROC glad_glGetVertexAttribdv = NULL;
PFNGLGETVERTEXATTRIBFVPROC glad_glGetVertexAttribfv = NULL;
PFNGLGETVERTEXATTRIBIVPROC glad_glGetVertexAttribiv = NULL;
PFNGLHINTPROC glad_glHint = NULL;
PFNGLISBUFFERPROC glad_glIsBuffer = NULL;
PFNGLISENABLEDPROC glad_glIsEnabled = NULL;
PFNGLISENABLEDIPROC glad_glIsEnabledi = NULL;
PFNGLISFRAMEBUFFERPROC glad_glIsFramebuffer = NULL;
PFNGLISPROGRAMPROC glad_glIsProgram = NULL;
PFNGLISQUERYPROC glad_glIsQuery = NULL;
PFNGLISRENDERBUFFERPROC glad_glIsRenderbuffer = NULL;
PFNGLISSAMPLERPROC glad_glIsSampler = NULL;
PFNGLISSHADERPROC glad_glIsShader = NULL;
PFNGLISSYNCPROC glad_glIsSync = NULL;
PFNGLISTEXTUREPROC glad_glIsTexture = NULL;
PFNGLISVERTEXARRAYPROC glad_glIsVertexArray = NULL;
PFNGLLINEWIDTHPROC glad_glLineWidth = NULL;
PFNGLLINKPROGRAMPROC glad_glLinkProgram = NULL;
PFNGLLOGICOPPROC glad_glLogicOp = NULL;
PFNGLMAPBUFFERPROC glad_glMapBuffer = NULL;
PFNGLMAPBUFFERRANGEPROC glad_glMapBufferRange = NULL;
PFNGLMULTIDRAWARRAYSPROC glad_glMultiDrawArrays = NULL;
PFNGLMULTIDRAWELEMENTSPROC glad_glMultiDrawElements = NULL;
PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC glad_glMultiDrawElementsBaseVertex = NULL;
PFNGLMULTITEXCOORDP1UIPROC glad_glMultiTexCoordP1ui = NULL;
PFNGLMULTITEXCOORDP1UIVPROC glad_glMultiTexCoordP1uiv = NULL;
PFNGLMULTITEXCOORDP2UIPROC glad_glMultiTexCoordP2ui = NULL;
PFNGLMULTITEXCOORDP2UIVPROC glad_glMultiTexCoordP2uiv = NULL;
PFNGLMULTITEXCOORDP3UIPROC glad_glMultiTexCoordP3ui = NULL;
PFNGLMULTITEXCOORDP3UIVPROC glad_glMultiTexCoordP3uiv = NULL;
PFNGLMULTITEXCOORDP4UIPROC glad_glMultiTexCoordP4ui = NULL;
PFNGLMULTITEXCOORDP4UIVPROC glad_glMultiTexCoordP4uiv = NULL;
PFNGLNORMALP3UIPROC glad_glNormalP3ui = NULL;
PFNGLNORMALP3UIVPROC glad_glNormalP3uiv = NULL;
PFNGLPIXELSTOREFPROC glad_glPixelStoref = NULL;
PFNGLPIXELSTOREIPROC glad_glPixelStorei = NULL;
PFNGLPOINTPARAMETERFPROC glad_glPointParameterf = NULL;
PFNGLPOINTPARAMETERFVPROC glad_glPointParameterfv = NULL;
PFNGLPOINTPARAMETERIPROC glad_glPointParameteri = NULL;
PFNGLPOINTPARAMETERIVPROC glad_glPointParameteriv = NULL;
PFNGLPOINTSIZEPROC glad_glPointSize = NULL;
PFNGLPOLYGONMODEPROC glad_glPolygonMode = NULL;
PFNGLPOLYGONOFFSETPROC glad_glPolygonOffset = NULL;
PFNGLPRIMITIVERESTARTINDEXPROC glad_glPrimitiveRestartIndex = NULL;
PFNGLPROVOKINGVERTEXPROC glad_glProvokingVertex = NULL;
PFNGLQUERYCOUNTERPROC glad_glQueryCounter = NULL;
PFNGLREADBUFFERPROC glad_glReadBuffer = NULL;
PFNGLREADPIXELSPROC glad_glReadPixels = NULL;
PFNGLRENDERBUFFERSTORAGEPROC glad_glRenderbufferStorage = NULL;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glad_glRenderbufferStorageMultisample = NULL;
PFNGLSAMPLECOVERAGEPROC glad_glSampleCoverage = NULL;
PFNGLSAMPLEMASKIPROC glad_glSampleMaski = NULL;
PFNGLSAMPLERPARAMETERIIVPROC glad_glSamplerParameterIiv = NULL;
PFNGLSAMPLERPARAMETERIUIVPROC glad_glSamplerParameterIuiv = NULL;
PFNGLSAMPLERPARAMETERFPROC glad_glSamplerParameterf = NULL;
PFNGLSAMPLERPARAMETERFVPROC glad_glSamplerParameterfv = NULL;
PFNGLSAMPLERPARAMETERIPROC glad_glSamplerParameteri = NULL;
PFNGLSAMPLERPARAMETERIVPROC glad_glSamplerParameteriv = NULL;
PFNGLSCISSORPROC glad_glScissor = NULL;
PFNGLSECONDARYCOLORP3UIPROC glad_glSecondaryColorP3ui = NULL;
PFNGLSECONDARYCOLORP3UIVPROC glad_glSecondaryColorP3uiv = NULL;
PFNGLSHADERSOURCEPROC glad_glShaderSource = NULL;
PFNGLSTENCILFUNCPROC glad_glStencilFunc = NULL;
PFNGLSTENCILFUNCSEPARATEPROC glad_glStencilFuncSeparate = NULL;
PFNGLSTENCILMASKPROC glad_glStencilMask = NULL;
PFNGLSTENCILMASKSEPARATEPROC glad_glStencilMaskSeparate = NULL;
PFNGLSTENCILOPPROC glad_glStencilOp = NULL;
PFNGLSTENCILOPSEPARATEPROC glad_glStencilOpSeparate = NULL;
PFNGLTEXBUFFERPROC glad_glTexBuffer = NULL;
PFNGLTEXCOORDP1UIPROC glad_glTexCoordP1ui = NULL;
PFNGLTEXCOORDP1UIVPROC glad_glTexCoordP1uiv = NULL;
PFNGLTEXCOORDP2UIPROC glad_glTexCoordP2ui = NULL;
PFNGLTEXCOORDP2UIVPROC glad_glTexCoordP2uiv = NULL;
PFNGLTEXCOORDP3UIPROC glad_glTexCoordP3ui = NULL;
PFNGLTEXCOORDP3UIVPROC glad_glTexCoordP3uiv = NULL;
PFNGLTEXCOORDP4UIPROC glad_glTexCoordP4ui = NULL;
PFNGLTEXCOORDP4UIVPROC glad_glTexCoordP4uiv = NULL;
PFNGLTEXIMAGE1DPROC glad_glTexImage1D = NULL;
PFNGLTEXIMAGE2DPROC glad_glTexImage2D = NULL;
PFNGLTEXIMAGE2DMULTISAMPLEPROC glad_glTexImage2DMultisample = NULL;
PFNGLTEXIMAGE3DPROC glad_glTexImage3D = NULL;
PFNGLTEXIMAGE3DMULTISAMPLEPROC glad_glTexImage3DMultisample = NULL;
PFNGLTEXPARAMETERIIVPROC glad_glTexParameterIiv = NULL;
PFNGLTEXPARAMETERIUIVPROC glad_glTexParameterIuiv = NULL;
PFNGLTEXPARAMETERFPROC glad_glTexParameterf = NULL;
PFNGLTEXPARAMETERFVPROC glad_glTexParameterfv = NULL;
PFNGLTEXPARAMETERIPROC glad_glTexParameteri = NULL;
PFNGLTEXPARAMETERIVPROC glad_glTexParameteriv = NULL;
PFNGLTEXSUBIMAGE1DPROC glad_glTexSubImage1D = NULL;
PFNGLTEXSUBIMAGE2DPROC glad_glTexSubImage2D = NULL;
PFNGLTEXSUBIMAGE3DPROC glad_glTexSubImage3D = NULL;
PFNGLTRANSFORMFEEDBACKVARYINGSPROC glad_glTransformFeedbackVaryings = NULL;
PFNGLUNIFORM1FPROC glad_glUniform1f = NULL;
PFNGLUNIFORM1FVPROC glad_glUniform1fv = NULL;
PFNGLUNIFORM1IPROC glad_glUniform1i = NULL;
PFNGLUNIFORM1IVPROC glad_glUniform1iv = NULL;
PFNGLUNIFORM1UIPROC glad_glUniform1ui = NULL;
PFNGLUNIFORM1UIVPROC glad_glUniform1uiv = NULL;
PFNGLUNIFORM2FPROC glad_glUniform2f = NULL;
PFNGLUNIFORM2FVPROC glad_glUniform2fv = NULL;
PFNGLUNIFORM2IPROC glad_glUniform2i = NULL;
PFNGLUNIFORM2IVPROC glad_glUniform2iv = NULL;
PFNGLUNIFORM2UIPROC glad_glUniform2ui = NULL;
PFNGLUNIFORM2UIVPROC glad_glUniform2uiv = NULL;
PFNGLUNIFORM3FPROC glad_glUniform3f = NULL;
PFNGLUNIFORM3FVPROC glad_glUniform3fv = NULL;
PFNGLUNIFORM3IPROC glad_glUniform3i = NULL;
PFNGLUNIFORM3IVPROC glad_glUniform3iv = NULL;
PFNGLUNIFORM3UIPROC glad_glUniform3ui = NULL;
PFNGLUNIFORM3UIVPROC glad_glUniform3uiv = NULL;
PFNGLUNIFORM4FPROC glad_glUniform4f = NULL;
PFNGLUNIFORM4FVPROC glad_glUniform4fv = NULL;
PFNGLUNIFORM4IPROC glad_glUniform4i = NULL;
PFNGLUNIFORM4IVPROC glad_glUniform4iv = NULL;
PFNGLUNIFORM4UIPROC glad_glUniform4ui = NULL;
PFNGLUNIFORM4UIVPROC glad_glUniform4uiv = NULL;
PFNGLUNIFORMBLOCKBINDINGPROC glad_glUniformBlockBinding = NULL;
PFNGLUNIFORMMATRIX2FVPROC glad_glUniformMatrix2fv = NULL;
PFNGLUNIFORMMATRIX2X3FVPROC glad_glUniformMatrix2x3fv = NULL;
PFNGLUNIFORMMATRIX2X4FVPROC glad_glUniformMatrix2x4fv = NULL;
PFNGLUNIFORMMATRIX3FVPROC glad_glUniformMatrix3fv = NULL;
PFNGLUNIFORMMATRIX3X2FVPROC glad_glUniformMatrix3x2fv = NULL;
PFNGLUNIFORMMATRIX3X4FVPROC glad_glUniformMatrix3x4fv = NULL;
PFNGLUNIFORMMATRIX4FVPROC glad_glUniformMatrix4fv = NULL;
PFNGLUNIFORMMATRIX4X2FVPROC glad_glUniformMatrix4x2fv = NULL;
PFNGLUNIFORMMATRIX4X3FVPROC glad_glUniformMatrix4x3fv = NULL;
PFNGLUNMAPBUFFERPROC glad_glUnmapBuffer = NULL;
PFNGLUSEPROGRAMPROC glad_glUseProgram = NULL;
PFNGLVALIDATEPROGRAMPROC glad_glValidateProgram = NULL;
PFNGLVERTEXATTRIB1DPROC glad_glVertexAttrib1d = NULL;
PFNGLVERTEXATTRIB1DVPROC glad_glVertexAttrib1dv = NULL;
PFNGLVERTEXATTRIB1FPROC glad_glVertexAttrib1f = NULL;
PFNGLVERTEXATTRIB1FVPROC glad_glVertexAttrib1fv = NULL;
PFNGLVERTEXATTRIB1SPROC glad_glVertexAttrib1s = NULL;
PFNGLVERTEXATTRIB1SVPROC glad_glVertexAttrib1sv = NULL;
PFNGLVERTEXATTRIB2DPROC glad_glVertexAttrib2d = NULL;
PFNGLVERTEXATTRIB2DVPROC glad_glVertexAttrib2dv = NULL;
PFNGLVERTEXATTRIB2FPROC glad_glVertexAttrib2f = NULL;
PFNGLVERTEXATTRIB2FVPROC glad_glVertexAttrib2fv = NULL;
PFNGLVERTEXATTRIB2SPROC glad_glVertexAttrib2s = NULL;
PFNGLVERTEXATTRIB2SVPROC glad_glVertexAttrib2sv = NULL;
PFNGLVERTEXATTRIB3DPROC glad_glVertexAttrib3d = NULL;
PFNGLVERTEXATTRIB3DVPROC glad_glVertexAttrib3dv = NULL;
PFNGLVERTEXATTRIB3FPROC glad_glVertexAttrib3f = NULL;
PFNGLVERTEXATTRIB3FVPROC glad_glVertexAttrib3fv = NULL;
PFNGLVERTEXATTRIB3SPROC glad_glVertexAttrib3s = NULL;
PFNGLVERTEXATTRIB3SVPROC glad_glVertexAttrib3sv = NULL;
PFNGLVERTEXATTRIB4NBVPROC glad_glVertexAttrib4Nbv = NULL;
PFNGLVERTEXATTRIB4NIVPROC glad_glVertexAttrib4Niv = NULL;
PFNGLVERTEXATTRIB4NSVPROC glad_glVertexAttrib4Nsv = NULL;
PFNGLVERTEXATTRIB4NUBPROC glad_glVertexAttrib4Nub = NULL;
PFNGLVERTEXATTRIB4NUBVPROC glad_glVertexAttrib4Nubv = NULL;
PFNGLVERTEXATTRIB4NUIVPROC glad_glVertexAttrib4Nuiv = NULL;
PFNGLVERTEXATTRIB4NUSVPROC glad_glVertexAttrib4Nusv = NULL;
PFNGLVERTEXATTRIB4BVPROC glad_glVertexAttrib4bv = NULL;
PFNGLVERTEXATTRIB4DPROC glad_glVertexAttrib4d = NULL;
PFNGLVERTEXATTRIB4DVPROC glad_glVertexAttrib4dv = NULL;
PFNGLVERTEXATTRIB4FPROC glad_glVertexAttrib4f = NULL;
PFNGLVERTEXATTRIB4FVPROC glad_glVertexAttrib4fv = NULL;
PFNGLVERTEXATTRIB4IVPROC glad_glVertexAttrib4iv = NULL;
PFNGLVERTEXATTRIB4SPROC glad_glVertexAttrib4s = NULL;
PFNGLVERTEXATTRIB4SVPROC glad_glVertexAttrib4sv = NULL;
PFNGLVERTEXATTRIB4UBVPROC glad_glVertexAttrib4ubv = NULL;
PFNGLVERTEXATTRIB4UIVPROC glad_glVertexAttrib4uiv = NULL;
PFNGLVERTEXATTRIB4USVPROC glad_glVertexAttrib4usv = NULL;
PFNGLVERTEXATTRIBDIVISORPROC glad_glVertexAttribDivisor = NULL;
PFNGLVERTEXATTRIBI1IPROC glad_glVertexAttribI1i = NULL;
PFNGLVERTEXATTRIBI1IVPROC glad_glVertexAttribI1iv = NULL;
PFNGLVERTEXATTRIBI1UIPROC glad_glVertexAttribI1ui = NULL;
PFNGLVERTEXATTRIBI1UIVPROC glad_glVertexAttribI1uiv = NULL;
PFNGLVERTEXATTRIBI2IPROC glad_glVertexAttribI2i = NULL;
PFNGLVERTEXATTRIBI2IVPROC glad_glVertexAttribI2iv = NULL;
PFNGLVERTEXATTRIBI2UIPROC glad_glVertexAttribI2ui = NULL;
PFNGLVERTEXATTRIBI2UIVPROC glad_glVertexAttribI2uiv = NULL;
PFNGLVERTEXATTRIBI3IPROC glad_glVertexAttribI3i = NULL;
PFNGLVERTEXATTRIBI3IVPROC glad_glVertexAttribI3iv = NULL;
PFNGLVERTEXATTRIBI3UIPROC glad_glVertexAttribI3ui = NULL;
PFNGLVERTEXATTRIBI3UIVPROC glad_glVertexAttribI3uiv = NULL;
PFNGLVERTEXATTRIBI4BVPROC glad_glVertexAttribI4bv = NULL;
PFNGLVERTEXATTRIBI4IPROC glad_glVertexAttribI4i = NULL;
PFNGLVERTEXATTRIBI4IVPROC glad_glVertexAttribI4iv = NULL;
PFNGLVERTEXATTRIBI4SVPROC glad_glVertexAttribI4sv = NULL;
PFNGLVERTEXATTRIBI4UBVPROC glad_glVertexAttribI4ubv = NULL;
PFNGLVERTEXATTRIBI4UIPROC glad_glVertexAttribI4ui = NULL;
PFNGLVERTEXATTRIBI4UIVPROC glad_glVertexAttribI4uiv = NULL;
PFNGLVERTEXATTRIBI4USVPROC glad_glVertexAttribI4usv = NULL;
PFNGLVERTEXATTRIBIPOINTERPROC glad_glVertexAttribIPointer = NULL;
PFNGLVERTEXATTRIBP1UIPROC glad_glVertexAttribP1ui = NULL;
PFNGLVERTEXATTRIBP1UIVPROC glad_glVertexAttribP1uiv = NULL;
PFNGLVERTEXATTRIBP2UIPROC glad_glVertexAttribP2ui = NULL;
PFNGLVERTEXATTRIBP2UIVPROC glad_glVertexAttribP2uiv = NULL;
PFNGLVERTEXATTRIBP3UIPROC glad_glVertexAttribP3ui = NULL;
PFNGLVERTEXATTRIBP3UIVPROC glad_glVertexAttribP3uiv = NULL;
PFNGLVERTEXATTRIBP4UIPROC glad_glVertexAttribP4ui = NULL;
PFNGLVERTEXATTRIBP4UIVPROC glad_glVertexAttribP4uiv = NULL;
PFNGLVERTEXATTRIBPOINTERPROC glad_glVertexAttribPointer = NULL;
PFNGLVERTEXP2UIPROC glad_glVertexP2ui = NULL;
PFNGLVERTEXP2UIVPROC glad_glVertexP2uiv = NULL;
PFNGLVERTEXP3UIPROC glad_glVertexP3ui = NULL;
PFNGLVERTEXP3UIVPROC glad_glVertexP3uiv = NULL;
PFNGLVERTEXP4UIPROC glad_glVertexP4ui = NULL;
PFNGLVERTEXP4UIVPROC glad_glVertexP4uiv = NULL;
PFNGLVIEWPORTPROC glad_glViewport = NULL;
PFNGLWAITSYNCPROC glad_glWaitSync = NULL;
static void load_GL_VERSION_1_0(GLADloadproc load) {
	if (!GLAD_GL_VERSION_1_0) return;
	glad_glCullFace = (PFNGLCULLFACEPROC) load("glCullFace");
	glad_glFrontFace = (PFNGLFRONTFACEPROC) load("glFrontFace");
	glad_glHint = (PFNGLHINTPROC) load("glHint");
	glad_glLineWidth = (PFNGLLINEWIDTHPROC) load("glLineWidth");
	glad_glPointSize = (PFNGLPOINTSIZEPROC) load("glPointSize");
	glad_glPolygonMode = (PFNGLPOLYGONMODEPROC) load("glPolygonMode");
	glad_glScissor = (PFNGLSCISSORPROC) load("glScissor");
	glad_glTexParameterf = (PFNGLTEXPARAMETERFPROC) load("glTexParameterf");
	glad_glTexParameterfv = (PFNGLTEXPARAMETERFVPROC) load("glTexParameterfv");
	glad_glTexParameteri = (PFNGLTEXPARAMETERIPROC) load("glTexParameteri");
	glad_glTexParameteriv = (PFNGLTEXPARAMETERIVPROC) load("glTexParameteriv");
	glad_glTexImage1D = (PFNGLTEXIMAGE1DPROC) load("glTexImage1D");
	glad_glTexImage2D = (PFNGLTEXIMAGE2DPROC) load("glTexImage2D");
	glad_glDrawBuffer = (PFNGLDRAWBUFFERPROC) load("glDrawBuffer");
	glad_glClear = (PFNGLCLEARPROC) load("glClear");
	glad_glClearColor = (PFNGLCLEARCOLORPROC) load("glClearColor");
	glad_glClearStencil = (PFNGLCLEARSTENCILPROC) load("glClearStencil");
	glad_glClearDepth = (PFNGLCLEARDEPTHPROC) load("glClearDepth");
	glad_glStencilMask = (PFNGLSTENCILMASKPROC) load("glStencilMask");
	glad_glColorMask = (PFNGLCOLORMASKPROC) load("glColorMask");
	glad_glDepthMask = (PFNGLDEPTHMASKPROC) load("glDepthMask");
	glad_glDisable = (PFNGLDISABLEPROC) load("glDisable");
	glad_glEnable = (PFNGLENABLEPROC) load("glEnable");
	glad_glFinish = (PFNGLFINISHPROC) load("glFinish");
	glad_glFlush = (PFNGLFLUSHPROC) load("glFlush");
	glad_glBlendFunc = (PFNGLBLENDFUNCPROC) load("glBlendFunc");
	glad_glLogicOp = (PFNGLLOGICOPPROC) load("glLogicOp");
	glad_glStencilFunc = (PFNGLSTENCILFUNCPROC) load("glStencilFunc");
	glad_glStencilOp = (PFNGLSTENCILOPPROC) load("glStencilOp");
	glad_glDepthFunc = (PFNGLDEPTHFUNCPROC) load("glDepthFunc");
	glad_glPixelStoref = (PFNGLPIXELSTOREFPROC) load("glPixelStoref");
	glad_glPixelStorei = (PFNGLPIXELSTOREIPROC) load("glPixelStorei");
	glad_glReadBuffer = (PFNGLREADBUFFERPROC) load("glReadBuffer");
	glad_glReadPixels = (PFNGLREADPIXELSPROC) load("glReadPixels");
	glad_glGetBooleanv = (PFNGLGETBOOLEANVPROC) load("glGetBooleanv");
	glad_glGetDoublev = (PFNGLGETDOUBLEVPROC) load("glGetDoublev");
	glad_glGetError = (PFNGLGETERRORPROC) load("glGetError");
	glad_glGetFloatv = (PFNGLGETFLOATVPROC) load("glGetFloatv");
	glad_glGetIntegerv = (PFNGLGETINTEGERVPROC) load("glGetIntegerv");
	glad_glGetString = (PFNGLGETSTRINGPROC) load("glGetString");
	glad_glGetTexImage = (PFNGLGETTEXIMAGEPROC) load("glGetTexImage");
	glad_glGetTexParameterfv = (PFNGLGETTEXPARAMETERFVPROC) load("glGetTexParameterfv");
	glad_glGetTexParameteriv = (PFNGLGETTEXPARAMETERIVPROC) load("glGetTexParameteriv");
	glad_glGetTexLevelParameterfv = (PFNGLGETTEXLEVELPARAMETERFVPROC) load("glGetTexLevelParameterfv");
	glad_glGetTexLevelParameteriv = (PFNGLGETTEXLEVELPARAMETERIVPROC) load("glGetTexLevelParameteriv");
	glad_glIsEnabled = (PFNGLISENABLEDPROC) load("glIsEnabled");
	glad_glDepthRange = (PFNGLDEPTHRANGEPROC) load("glDepthRange");
	glad_glViewport = (PFNGLVIEWPORTPROC) load("glViewport");
}
static void load_GL_VERSION_1_1(GLADloadproc load) {
	if (!GLAD_GL_VERSION_1_1) return;
	glad_glDrawArrays = (PFNGLDRAWARRAYSPROC) load("glDrawArrays");
	glad_glDrawElements = (PFNGLDRAWELEMENTSPROC) load("glDrawElements");
	glad_glPolygonOffset = (PFNGLPOLYGONOFFSETPROC) load("glPolygonOffset");
	glad_glCopyTexImage1D = (PFNGLCOPYTEXIMAGE1DPROC) load("glCopyTexImage1D");
	glad_glCopyTexImage2D = (PFNGLCOPYTEXIMAGE2DPROC) load("glCopyTexImage2D");
	glad_glCopyTexSubImage1D = (PFNGLCOPYTEXSUBIMAGE1DPROC) load("glCopyTexSubImage1D");
	glad_glCopyTexSubImage2D = (PFNGLCOPYTEXSUBIMAGE2DPROC) load("glCopyTexSubImage2D");
	glad_glTexSubImage1D = (PFNGLTEXSUBIMAGE1DPROC) load("glTexSubImage1D");
	glad_glTexSubImage2D = (PFNGLTEXSUBIMAGE2DPROC) load("glTexSubImage2D");
	glad_glBindTexture = (PFNGLBINDTEXTUREPROC) load("glBindTexture");
	glad_glDeleteTextures = (PFNGLDELETETEXTURESPROC) load("glDeleteTextures");
	glad_glGenTextures = (PFNGLGENTEXTURESPROC) load("glGenTextures");
	glad_glIsTexture = (PFNGLISTEXTUREPROC) load("glIsTexture");
}
static void load_GL_VERSION_1_2(GLADloadproc load) {
	if (!GLAD_GL_VERSION_1_2) return;
	glad_glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC) load("glDrawRangeElements");
	glad_glTexImage3D = (PFNGLTEXIMAGE3DPROC) load("glTexImage3D");
	glad_glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC) load("glTexSubImage3D");
	glad_glCopyTexSubImage3D = (PFNGLCOPYTEXSUBIMAGE3DPROC) load("glCopyTexSubImage3D");
}
static void load_GL_VERSION_1_3(GLADloadproc load) {
	if (!GLAD_GL_VERSION_1_3) return;
	glad_glActiveTexture = (PFNGLACTIVETEXTUREPROC) load("glActiveTexture");
	glad_glSampleCoverage = (PFNGLSAMPLECOVERAGEPROC) load("glSampleCoverage");
	glad_glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC) load("glCompressedTexImage3D");
	glad_glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC) load("glCompressedTexImage2D");
	glad_glCompressedTexImage1D = (PFNGLCOMPRESSEDTEXIMAGE1DPROC) load("glCompressedTexImage1D");
	glad_glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) load("glCompressedTexSubImage3D");
	glad_glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) load("glCompressedTexSubImage2D");
	glad_glCompressedTexSubImage1D = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC) load("glCompressedTexSubImage1D");
	glad_glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC) load("glGetCompressedTexImage");
}
static void load_GL_VERSION_1_4(GLADloadproc load) {
	if (!GLAD_GL_VERSION_1_4) return;
	glad_glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC) load("glBlendFuncSeparate");
	glad_glMultiDrawArrays = (PFNGLMULTIDRAWARRAYSPROC) load("glMultiDrawArrays");
	glad_glMultiDrawElements = (PFNGLMULTIDRAWELEMENTSPROC) load("glMultiDrawElements");
	glad_glPointParameterf = (PFNGLPOINTPARAMETERFPROC) load("glPointParameterf");
	glad_glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC) load("glPointParameterfv");
	glad_glPointParameteri = (PFNGLPOINTPARAMETERIPROC) load("glPointParameteri");
	glad_glPointParameteriv = (PFNGLPOINTPARAMETERIVPROC) load("glPointParameteriv");
	glad_glBlendColor = (PFNGLBLENDCOLORPROC) load("glBlendColor");
	glad_glBlendEquation = (PFNGLBLENDEQUATIONPROC) load("glBlendEquation");
}
static void load_GL_VERSION_1_5(GLADloadproc load) {
	if (!GLAD_GL_VERSION_1_5) return;
	glad_glGenQueries = (PFNGLGENQUERIESPROC) load("glGenQueries");
	glad_glDeleteQueries = (PFNGLDELETEQUERIESPROC) load("glDeleteQueries");
	glad_glIsQuery = (PFNGLISQUERYPROC) load("glIsQuery");
	glad_glBeginQuery = (PFNGLBEGINQUERYPROC) load("glBeginQuery");
	glad_glEndQuery = (PFNGLENDQUERYPROC) load("glEndQuery");
	glad_glGetQueryiv = (PFNGLGETQUERYIVPROC) load("glGetQueryiv");
	glad_glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC) load("glGetQueryObjectiv");
	glad_glGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC) load("glGetQueryObjectuiv");
	glad_glBindBuffer = (PFNGLBINDBUFFERPROC) load("glBindBuffer");
	glad_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) load("glDeleteBuffers");
	glad_glGenBuffers = (PFNGLGENBUFFERSPROC) load("glGenBuffers");
	glad_glIsBuffer = (PFNGLISBUFFERPROC) load("glIsBuffer");
	glad_glBufferData = (PFNGLBUFFERDATAPROC) load("glBufferData");
	glad_glBufferSubData = (PFNGLBUFFERSUBDATAPROC) load("glBufferSubData");
	glad_glGetBufferSubData = (PFNGLGETBUFFERSUBDATAPROC) load("glGetBufferSubData");
	glad_glMapBuffer = (PFNGLMAPBUFFERPROC) load("glMapBuffer");
	glad_glUnmapBuffer = (PFNGLUNMAPBUFFERPROC) load("glUnmapBuffer");
	glad_glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC) load("glGetBufferParameteriv");
	glad_glGetBufferPointerv = (PFNGLGETBUFFERPOINTERVPROC) load("glGetBufferPointerv");
}
static void load_GL_VERSION_2_0(GLADloadproc load) {
	if (!GLAD_GL_VERSION_2_0) return;
	glad_glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC) load("glBlendEquationSeparate");
	glad_glDrawBuffers = (PFNGLDRAWBUFFERSPROC) load("glDrawBuffers");
	glad_glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC) load("glStencilOpSeparate");
	glad_glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC) load("glStencilFuncSeparate");
	glad_glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC) load("glStencilMaskSeparate");
	glad_glAttachShader = (PFNGLATTACHSHADERPROC) load("glAttachShader");
	glad_glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC) load("glBindAttribLocation");
	glad_glCompileShader = (PFNGLCOMPILESHADERPROC) load("glCompileShader");
	glad_glCreateProgram = (PFNGLCREATEPROGRAMPROC) load("glCreateProgram");
	glad_glCreateShader = (PFNGLCREATESHADERPROC) load("glCreateShader");
	glad_glDeleteProgram = (PFNGLDELETEPROGRAMPROC) load("glDeleteProgram");
	glad_glDeleteShader = (PFNGLDELETESHADERPROC) load("glDeleteShader");
	glad_glDetachShader = (PFNGLDETACHSHADERPROC) load("glDetachShader");
	glad_glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) load("glDisableVertexAttribArray");
	glad_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) load("glEnableVertexAttribArray");
	glad_glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC) load("glGetActiveAttrib");
	glad_glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC) load("glGetActiveUniform");
	glad_glGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC) load("glGetAttachedShaders");
	glad_glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) load("glGetAttribLocation");
	glad_glGetProgramiv = (PFNGLGETPROGRAMIVPROC) load("glGetProgramiv");
	glad_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) load("glGetProgramInfoLog");
	glad_glGetShaderiv = (PFNGLGETSHADERIVPROC) load("glGetShaderiv");
	glad_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) load("glGetShaderInfoLog");
	glad_glGetShaderSource = (PFNGLGETSHADERSOURCEPROC) load("glGetShaderSource");
	glad_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) load("glGetUniformLocation");
	glad_glGetUniformfv = (PFNGLGETUNIFORMFVPROC) load("glGetUniformfv");
	glad_glGetUniformiv = (PFNGLGETUNIFORMIVPROC) load("glGetUniformiv");
	glad_glGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC) load("glGetVertexAttribdv");
	glad_glGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC) load("glGetVertexAttribfv");
	glad_glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC) load("glGetVertexAttribiv");
	glad_glGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC) load("glGetVertexAttribPointerv");
	glad_glIsProgram = (PFNGLISPROGRAMPROC) load("glIsProgram");
	glad_glIsShader = (PFNGLISSHADERPROC) load("glIsShader");
	glad_glLinkProgram = (PFNGLLINKPROGRAMPROC) load("glLinkProgram");
	glad_glShaderSource = (PFNGLSHADERSOURCEPROC) load("glShaderSource");
	glad_glUseProgram = (PFNGLUSEPROGRAMPROC) load("glUseProgram");
	glad_glUniform1f = (PFNGLUNIFORM1FPROC) load("glUniform1f");
	glad_glUniform2f = (PFNGLUNIFORM2FPROC) load("glUniform2f");
	glad_glUniform3f = (PFNGLUNIFORM3FPROC) load("glUniform3f");
	glad_glUniform4f = (PFNGLUNIFORM4FPROC) load("glUniform4f");
	glad_glUniform1i = (PFNGLUNIFORM1IPROC) load("glUniform1i");
	glad_glUniform2i = (PFNGLUNIFORM2IPROC) load("glUniform2i");
	glad_glUniform3i = (PFNGLUNIFORM3IPROC) load("glUniform3i");
	glad_glUniform4i = (PFNGLUNIFORM4IPROC) load("glUniform4i");
	glad_glUniform1fv = (PFNGLUNIFORM1FVPROC) load("glUniform1fv");
	glad_glUniform2fv = (PFNGLUNIFORM2FVPROC) load("glUniform2fv");
	glad_glUniform3fv = (PFNGLUNIFORM3FVPROC) load("glUniform3fv");
	glad_glUniform4fv = (PFNGLUNIFORM4FVPROC) load("glUniform4fv");
	glad_glUniform1iv = (PFNGLUNIFORM1IVPROC) load("glUniform1iv");
	glad_glUniform2iv = (PFNGLUNIFORM2IVPROC) load("glUniform2iv");
	glad_glUniform3iv = (PFNGLUNIFORM3IVPROC) load("glUniform3iv");
	glad_glUniform4iv = (PFNGLUNIFORM4IVPROC) load("glUniform4iv");
	glad_glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC) load("glUniformMatrix2fv");
	glad_glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC) load("glUniformMatrix3fv");
	glad_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) load("glUniformMatrix4fv");
	glad_glValidateProgram = (PFNGLVALIDATEPROGRAMPROC) load("glValidateProgram");
	glad_glVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC) load("glVertexAttrib1d");
	glad_glVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC) load("glVertexAttrib1dv");
	glad_glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC) load("glVertexAttrib1f");
	glad_glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC) load("glVertexAttrib1fv");
	glad_glVertexAttrib1s = (PFNGLVERTEXATTRIB1SPROC) load("glVertexAttrib1s");
	glad_glVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC) load("glVertexAttrib1sv");
	glad_glVertexAttrib2d = (PFNGLVERTEXATTRIB2DPROC) load("glVertexAttrib2d");
	glad_glVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC) load("glVertexAttrib2dv");
	glad_glVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC) load("glVertexAttrib2f");
	glad_glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC) load("glVertexAttrib2fv");
	glad_glVertexAttrib2s = (PFNGLVERTEXATTRIB2SPROC) load("glVertexAttrib2s");
	glad_glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC) load("glVertexAttrib2sv");
	glad_glVertexAttrib3d = (PFNGLVERTEXATTRIB3DPROC) load("glVertexAttrib3d");
	glad_glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC) load("glVertexAttrib3dv");
	glad_glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC) load("glVertexAttrib3f");
	glad_glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC) load("glVertexAttrib3fv");
	glad_glVertexAttrib3s = (PFNGLVERTEXATTRIB3SPROC) load("glVertexAttrib3s");
	glad_glVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC) load("glVertexAttrib3sv");
	glad_glVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBVPROC) load("glVertexAttrib4Nbv");
	glad_glVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIVPROC) load("glVertexAttrib4Niv");
	glad_glVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSVPROC) load("glVertexAttrib4Nsv");
	glad_glVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC) load("glVertexAttrib4Nub");
	glad_glVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC) load("glVertexAttrib4Nubv");
	glad_glVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIVPROC) load("glVertexAttrib4Nuiv");
	glad_glVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSVPROC) load("glVertexAttrib4Nusv");
	glad_glVertexAttrib4bv = (PFNGLVERTEXATTRIB4BVPROC) load("glVertexAttrib4bv");
	glad_glVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC) load("glVertexAttrib4d");
	glad_glVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC) load("glVertexAttrib4dv");
	glad_glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC) load("glVertexAttrib4f");
	glad_glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC) load("glVertexAttrib4fv");
	glad_glVertexAttrib4iv = (PFNGLVERTEXATTRIB4IVPROC) load("glVertexAttrib4iv");
	glad_glVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC) load("glVertexAttrib4s");
	glad_glVertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC) load("glVertexAttrib4sv");
	glad_glVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBVPROC) load("glVertexAttrib4ubv");
	glad_glVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIVPROC) load("glVertexAttrib4uiv");
	glad_glVertexAttrib4usv = (PFNGLVERTEXATTRIB4USVPROC) load("glVertexAttrib4usv");
	glad_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) load("glVertexAttribPointer");
}
static void load_GL_VERSION_2_1(GLADloadproc load) {
	if (!GLAD_GL_VERSION_2_1) return;
	glad_glUniformMatrix2x3fv = (PFNGLUNIFORMMATRIX2X3FVPROC) load("glUniformMatrix2x3fv");
	glad_glUniformMatrix3x2fv = (PFNGLUNIFORMMATRIX3X2FVPROC) load("glUniformMatrix3x2fv");
	glad_glUniformMatrix2x4fv = (PFNGLUNIFORMMATRIX2X4FVPROC) load("glUniformMatrix2x4fv");
	glad_glUniformMatrix4x2fv = (PFNGLUNIFORMMATRIX4X2FVPROC) load("glUniformMatrix4x2fv");
	glad_glUniformMatrix3x4fv = (PFNGLUNIFORMMATRIX3X4FVPROC) load("glUniformMatrix3x4fv");
	glad_glUniformMatrix4x3fv = (PFNGLUNIFORMMATRIX4X3FVPROC) load("glUniformMatrix4x3fv");
}
static void load_GL_VERSION_3_0(GLADloadproc load) {
	if (!GLAD_GL_VERSION_3_0) return;
	glad_glColorMaski = (PFNGLCOLORMASKIPROC) load("glColorMaski");
	glad_glGetBooleani_v = (PFNGLGETBOOLEANI_VPROC) load("glGetBooleani_v");
	glad_glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC) load("glGetIntegeri_v");
	glad_glEnablei = (PFNGLENABLEIPROC) load("glEnablei");
	glad_glDisablei = (PFNGLDISABLEIPROC) load("glDisablei");
	glad_glIsEnabledi = (PFNGLISENABLEDIPROC) load("glIsEnabledi");
	glad_glBeginTransformFeedback = (PFNGLBEGINTRANSFORMFEEDBACKPROC) load("glBeginTransformFeedback");
	glad_glEndTransformFeedback = (PFNGLENDTRANSFORMFEEDBACKPROC) load("glEndTransformFeedback");
	glad_glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC) load("glBindBufferRange");
	glad_glBindBufferBase = (PFNGLBINDBUFFERBASEPROC) load("glBindBufferBase");
	glad_glTransformFeedbackVaryings = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC) load("glTransformFeedbackVaryings");
	glad_glGetTransformFeedbackVarying = (PFNGLGETTRANSFORMFEEDBACKVARYINGPROC) load("glGetTransformFeedbackVarying");
	glad_glClampColor = (PFNGLCLAMPCOLORPROC) load("glClampColor");
	glad_glBeginConditionalRender = (PFNGLBEGINCONDITIONALRENDERPROC) load("glBeginConditionalRender");
	glad_glEndConditionalRender = (PFNGLENDCONDITIONALRENDERPROC) load("glEndConditionalRender");
	glad_glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC) load("glVertexAttribIPointer");
	glad_glGetVertexAttribIiv = (PFNGLGETVERTEXATTRIBIIVPROC) load("glGetVertexAttribIiv");
	glad_glGetVertexAttribIuiv = (PFNGLGETVERTEXATTRIBIUIVPROC) load("glGetVertexAttribIuiv");
	glad_glVertexAttribI1i = (PFNGLVERTEXATTRIBI1IPROC) load("glVertexAttribI1i");
	glad_glVertexAttribI2i = (PFNGLVERTEXATTRIBI2IPROC) load("glVertexAttribI2i");
	glad_glVertexAttribI3i = (PFNGLVERTEXATTRIBI3IPROC) load("glVertexAttribI3i");
	glad_glVertexAttribI4i = (PFNGLVERTEXATTRIBI4IPROC) load("glVertexAttribI4i");
	glad_glVertexAttribI1ui = (PFNGLVERTEXATTRIBI1UIPROC) load("glVertexAttribI1ui");
	glad_glVertexAttribI2ui = (PFNGLVERTEXATTRIBI2UIPROC) load("glVertexAttribI2ui");
	glad_glVertexAttribI3ui = (PFNGLVERTEXATTRIBI3UIPROC) load("glVertexAttribI3ui");
	glad_glVertexAttribI4ui = (PFNGLVERTEXATTRIBI4UIPROC) load("glVertexAttribI4ui");
	glad_glVertexAttribI1iv = (PFNGLVERTEXATTRIBI1IVPROC) load("glVertexAttribI1iv");
	glad_glVertexAttribI2iv = (PFNGLVERTEXATTRIBI2IVPROC) load("glVertexAttribI2iv");
	glad_glVertexAttribI3iv = (PFNGLVERTEXATTRIBI3IVPROC) load("glVertexAttribI3iv");
	glad_glVertexAttribI4iv = (PFNGLVERTEXATTRIBI4IVPROC) load("glVertexAttribI4iv");
	glad_glVertexAttribI1uiv = (PFNGLVERTEXATTRIBI1UIVPROC) load("glVertexAttribI1uiv");
	glad_glVertexAttribI2uiv = (PFNGLVERTEXATTRIBI2UIVPROC) load("glVertexAttribI2uiv");
	glad_glVertexAttribI3uiv = (PFNGLVERTEXATTRIBI3UIVPROC) load("glVertexAttribI3uiv");
	glad_glVertexAttribI4uiv = (PFNGLVERTEXATTRIBI4UIVPROC) load("glVertexAttribI4uiv");
	glad_glVertexAttribI4bv = (PFNGLVERTEXATTRIBI4BVPROC) load("glVertexAttribI4bv");
	glad_glVertexAttribI4sv = (PFNGLVERTEXATTRIBI4SVPROC) load("glVertexAttribI4sv");
	glad_glVertexAttribI4ubv = (PFNGLVERTEXATTRIBI4UBVPROC) load("glVertexAttribI4ubv");
	glad_glVertexAttribI4usv = (PFNGLVERTEXATTRIBI4USVPROC) load("glVertexAttribI4usv");
	glad_glGetUniformuiv = (PFNGLGETUNIFORMUIVPROC) load("glGetUniformuiv");
	glad_glBindFragDataLocation = (PFNGLBINDFRAGDATALOCATIONPROC) load("glBindFragDataLocation");
	glad_glGetFragDataLocation = (PFNGLGETFRAGDATALOCATIONPROC) load("glGetFragDataLocation");
	glad_glUniform1ui = (PFNGLUNIFORM1UIPROC) load("glUniform1ui");
	glad_glUniform2ui = (PFNGLUNIFORM2UIPROC) load("glUniform2ui");
	glad_glUniform3ui = (PFNGLUNIFORM3UIPROC) load("glUniform3ui");
	glad_glUniform4ui = (PFNGLUNIFORM4UIPROC) load("glUniform4ui");
	glad_glUniform1uiv = (PFNGLUNIFORM1UIVPROC) load("glUniform1uiv");
	glad_glUniform2uiv = (PFNGLUNIFORM2UIVPROC) load("glUniform2uiv");
	glad_glUniform3uiv = (PFNGLUNIFORM3UIVPROC) load("glUniform3uiv");
	glad_glUniform4uiv = (PFNGLUNIFORM4UIVPROC) load("glUniform4uiv");
	glad_glTexParameterIiv = (PFNGLTEXPARAMETERIIVPROC) load("glTexParameterIiv");
	glad_glTexParameterIuiv = (PFNGLTEXPARAMETERIUIVPROC) load("glTexParameterIuiv");
	glad_glGetTexParameterIiv = (PFNGLGETTEXPARAMETERIIVPROC) load("glGetTexParameterIiv");
	glad_glGetTexParameterIuiv = (PFNGLGETTEXPARAMETERIUIVPROC) load("glGetTexParameterIuiv");
	glad_glClearBufferiv = (PFNGLCLEARBUFFERIVPROC) load("glClearBufferiv");
	glad_glClearBufferuiv = (PFNGLCLEARBUFFERUIVPROC) load("glClearBufferuiv");
	glad_glClearBufferfv = (PFNGLCLEARBUFFERFVPROC) load("glClearBufferfv");
	glad_glClearBufferfi = (PFNGLCLEARBUFFERFIPROC) load("glClearBufferfi");
	glad_glGetStringi = (PFNGLGETSTRINGIPROC) load("glGetStringi");
	glad_glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC) load("glIsRenderbuffer");
	glad_glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC) load("glBindRenderbuffer");
	glad_glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC) load("glDeleteRenderbuffers");
	glad_glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC) load("glGenRenderbuffers");
	glad_glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC) load("glRenderbufferStorage");
	glad_glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) load("glGetRenderbufferParameteriv");
	glad_glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC) load("glIsFramebuffer");
	glad_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) load("glBindFramebuffer");
	glad_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) load("glDeleteFramebuffers");
	glad_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) load("glGenFramebuffers");
	glad_glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) load("glCheckFramebufferStatus");
	glad_glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC) load("glFramebufferTexture1D");
	glad_glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) load("glFramebufferTexture2D");
	glad_glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC) load("glFramebufferTexture3D");
	glad_glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) load("glFramebufferRenderbuffer");
	glad_glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) load("glGetFramebufferAttachmentParameteriv");
	glad_glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) load("glGenerateMipmap");
	glad_glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC) load("glBlitFramebuffer");
	glad_glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC) load("glRenderbufferStorageMultisample");
	glad_glFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC) load("glFramebufferTextureLayer");
	glad_glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC) load("glMapBufferRange");
	glad_glFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC) load("glFlushMappedBufferRange");
	glad_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC) load("glBindVertexArray");
	glad_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC) load("glDeleteVertexArrays");
	glad_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC) load("glGenVertexArrays");
	glad_glIsVertexArray = (PFNGLISVERTEXARRAYPROC) load("glIsVertexArray");
}
static void load_GL_VERSION_3_1(GLADloadproc load) {
	if (!GLAD_GL_VERSION_3_1) return;
	glad_glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC) load("glDrawArraysInstanced");
	glad_glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC) load("glDrawElementsInstanced");
	glad_glTexBuffer = (PFNGLTEXBUFFERPROC) load("glTexBuffer");
	glad_glPrimitiveRestartIndex = (PFNGLPRIMITIVERESTARTINDEXPROC) load("glPrimitiveRestartIndex");
	glad_glCopyBufferSubData = (PFNGLCOPYBUFFERSUBDATAPROC) load("glCopyBufferSubData");
	glad_glGetUniformIndices = (PFNGLGETUNIFORMINDICESPROC) load("glGetUniformIndices");
	glad_glGetActiveUniformsiv = (PFNGLGETACTIVEUNIFORMSIVPROC) load("glGetActiveUniformsiv");
	glad_glGetActiveUniformName = (PFNGLGETACTIVEUNIFORMNAMEPROC) load("glGetActiveUniformName");
	glad_glGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC) load("glGetUniformBlockIndex");
	glad_glGetActiveUniformBlockiv = (PFNGLGETACTIVEUNIFORMBLOCKIVPROC) load("glGetActiveUniformBlockiv");
	glad_glGetActiveUniformBlockName = (PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC) load("glGetActiveUniformBlockName");
	glad_glUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC) load("glUniformBlockBinding");
	glad_glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC) load("glBindBufferRange");
	glad_glBindBufferBase = (PFNGLBINDBUFFERBASEPROC) load("glBindBufferBase");
	glad_glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC) load("glGetIntegeri_v");
}
static void load_GL_VERSION_3_2(GLADloadproc load) {
	if (!GLAD_GL_VERSION_3_2) return;
	glad_glDrawElementsBaseVertex = (PFNGLDRAWELEMENTSBASEVERTEXPROC) load("glDrawElementsBaseVertex");
	glad_glDrawRangeElementsBaseVertex = (PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC) load("glDrawRangeElementsBaseVertex");
	glad_glDrawElementsInstancedBaseVertex = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC) load("glDrawElementsInstancedBaseVertex");
	glad_glMultiDrawElementsBaseVertex = (PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC) load("glMultiDrawElementsBaseVertex");
	glad_glProvokingVertex = (PFNGLPROVOKINGVERTEXPROC) load("glProvokingVertex");
	glad_glFenceSync = (PFNGLFENCESYNCPROC) load("glFenceSync");
	glad_glIsSync = (PFNGLISSYNCPROC) load("glIsSync");
	glad_glDeleteSync = (PFNGLDELETESYNCPROC) load("glDeleteSync");
	glad_glClientWaitSync = (PFNGLCLIENTWAITSYNCPROC) load("glClientWaitSync");
	glad_glWaitSync = (PFNGLWAITSYNCPROC) load("glWaitSync");
	glad_glGetInteger64v = (PFNGLGETINTEGER64VPROC) load("glGetInteger64v");
	glad_glGetSynciv = (PFNGLGETSYNCIVPROC) load("glGetSynciv");
	glad_glGetInteger64i_v = (PFNGLGETINTEGER64I_VPROC) load("glGetInteger64i_v");
	glad_glGetBufferParameteri64v = (PFNGLGETBUFFERPARAMETERI64VPROC) load("glGetBufferParameteri64v");
	glad_glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC) load("glFramebufferTexture");
	glad_glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC) load("glTexImage2DMultisample");
	glad_glTexImage3DMultisample = (PFNGLTEXIMAGE3DMULTISAMPLEPROC) load("glTexImage3DMultisample");
	glad_glGetMultisamplefv = (PFNGLGETMULTISAMPLEFVPROC) load("glGetMultisamplefv");
	glad_glSampleMaski = (PFNGLSAMPLEMASKIPROC) load("glSampleMaski");
}
static void load_GL_VERSION_3_3(GLADloadproc load) {
	if (!GLAD_GL_VERSION_3_3) return;
	glad_glBindFragDataLocationIndexed = (PFNGLBINDFRAGDATALOCATIONINDEXEDPROC) load("glBindFragDataLocationIndexed");
	glad_glGetFragDataIndex = (PFNGLGETFRAGDATAINDEXPROC) load("glGetFragDataIndex");
	glad_glGenSamplers = (PFNGLGENSAMPLERSPROC) load("glGenSamplers");
	glad_glDeleteSamplers = (PFNGLDELETESAMPLERSPROC) load("glDeleteSamplers");
	glad_glIsSampler = (PFNGLISSAMPLERPROC) load("glIsSampler");
	glad_glBindSampler = (PFNGLBINDSAMPLERPROC) load("glBindSampler");
	glad_glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC) load("glSamplerParameteri");
	glad_glSamplerParameteriv = (PFNGLSAMPLERPARAMETERIVPROC) load("glSamplerParameteriv");
	glad_glSamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC) load("glSamplerParameterf");
	glad_glSamplerParameterfv = (PFNGLSAMPLERPARAMETERFVPROC) load("glSamplerParameterfv");
	glad_glSamplerParameterIiv = (PFNGLSAMPLERPARAMETERIIVPROC) load("glSamplerParameterIiv");
	glad_glSamplerParameterIuiv = (PFNGLSAMPLERPARAMETERIUIVPROC) load("glSamplerParameterIuiv");
	glad_glGetSamplerParameteriv = (PFNGLGETSAMPLERPARAMETERIVPROC) load("glGetSamplerParameteriv");
	glad_glGetSamplerParameterIiv = (PFNGLGETSAMPLERPARAMETERIIVPROC) load("glGetSamplerParameterIiv");
	glad_glGetSamplerParameterfv = (PFNGLGETSAMPLERPARAMETERFVPROC) load("glGetSamplerParameterfv");
	glad_glGetSamplerParameterIuiv = (PFNGLGETSAMPLERPARAMETERIUIVPROC) load("glGetSamplerParameterIuiv");
	glad_glQueryCounter = (PFNGLQUERYCOUNTERPROC) load("glQueryCounter");
	glad_glGetQueryObjecti64v = (PFNGLGETQUERYOBJECTI64VPROC) load("glGetQueryObjecti64v");
	glad_glGetQueryObjectui64v = (PFNGLGETQUERYOBJECTUI64VPROC) load("glGetQueryObjectui64v");
	glad_glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC) load("glVertexAttribDivisor");
	glad_glVertexAttribP1ui = (PFNGLVERTEXATTRIBP1UIPROC) load("glVertexAttribP1ui");
	glad_glVertexAttribP1uiv = (PFNGLVERTEXATTRIBP1UIVPROC) load("glVertexAttribP1uiv");
	glad_glVertexAttribP2ui = (PFNGLVERTEXATTRIBP2UIPROC) load("glVertexAttribP2ui");
	glad_glVertexAttribP2uiv = (PFNGLVERTEXATTRIBP2UIVPROC) load("glVertexAttribP2uiv");
	glad_glVertexAttribP3ui = (PFNGLVERTEXATTRIBP3UIPROC) load("glVertexAttribP3ui");
	glad_glVertexAttribP3uiv = (PFNGLVERTEXATTRIBP3UIVPROC) load("glVertexAttribP3uiv");
	glad_glVertexAttribP4ui = (PFNGLVERTEXATTRIBP4UIPROC) load("glVertexAttribP4ui");
	glad_glVertexAttribP4uiv = (PFNGLVERTEXATTRIBP4UIVPROC) load("glVertexAttribP4uiv");
	glad_glVertexP2ui = (PFNGLVERTEXP2UIPROC) load("glVertexP2ui");
	glad_glVertexP2uiv = (PFNGLVERTEXP2UIVPROC) load("glVertexP2uiv");
	glad_glVertexP3ui = (PFNGLVERTEXP3UIPROC) load("glVertexP3ui");
	glad_glVertexP3uiv = (PFNGLVERTEXP3UIVPROC) load("glVertexP3uiv");
	glad_glVertexP4ui = (PFNGLVERTEXP4UIPROC) load("glVertexP4ui");
	glad_glVertexP4uiv = (PFNGLVERTEXP4UIVPROC) load("glVertexP4uiv");
	glad_glTexCoordP1ui = (PFNGLTEXCOORDP1UIPROC) load("glTexCoordP1ui");
	glad_glTexCoordP1uiv = (PFNGLTEXCOORDP1UIVPROC) load("glTexCoordP1uiv");
	glad_glTexCoordP2ui = (PFNGLTEXCOORDP2UIPROC) load("glTexCoordP2ui");
	glad_glTexCoordP2uiv = (PFNGLTEXCOORDP2UIVPROC) load("glTexCoordP2uiv");
	glad_glTexCoordP3ui = (PFNGLTEXCOORDP3UIPROC) load("glTexCoordP3ui");
	glad_glTexCoordP3uiv = (PFNGLTEXCOORDP3UIVPROC) load("glTexCoordP3uiv");
	glad_glTexCoordP4ui = (PFNGLTEXCOORDP4UIPROC) load("glTexCoordP4ui");
	glad_glTexCoordP4uiv = (PFNGLTEXCOORDP4UIVPROC) load("glTexCoordP4uiv");
	glad_glMultiTexCoordP1ui = (PFNGLMULTITEXCOORDP1UIPROC) load("glMultiTexCoordP1ui");
	glad_glMultiTexCoordP1uiv = (PFNGLMULTITEXCOORDP1UIVPROC) load("glMultiTexCoordP1uiv");
	glad_glMultiTexCoordP2ui = (PFNGLMULTITEXCOORDP2UIPROC) load("glMultiTexCoordP2ui");
	glad_glMultiTexCoordP2uiv = (PFNGLMULTITEXCOORDP2UIVPROC) load("glMultiTexCoordP2uiv");
	glad_glMultiTexCoordP3ui = (PFNGLMULTITEXCOORDP3UIPROC) load("glMultiTexCoordP3ui");
	glad_glMultiTexCoordP3uiv = (PFNGLMULTITEXCOORDP3UIVPROC) load("glMultiTexCoordP3uiv");
	glad_glMultiTexCoordP4ui = (PFNGLMULTITEXCOORDP4UIPROC) load("glMultiTexCoordP4ui");
	glad_glMultiTexCoordP4uiv = (PFNGLMULTITEXCOORDP4UIVPROC) load("glMultiTexCoordP4uiv");
	glad_glNormalP3ui = (PFNGLNORMALP3UIPROC) load("glNormalP3ui");
	glad_glNormalP3uiv = (PFNGLNORMALP3UIVPROC) load("glNormalP3uiv");
	glad_glColorP3ui = (PFNGLCOLORP3UIPROC) load("glColorP3ui");
	glad_glColorP3uiv = (PFNGLCOLORP3UIVPROC) load("glColorP3uiv");
	glad_glColorP4ui = (PFNGLCOLORP4UIPROC) load("glColorP4ui");
	glad_glColorP4uiv = (PFNGLCOLORP4UIVPROC) load("glColorP4uiv");
	glad_glSecondaryColorP3ui = (PFNGLSECONDARYCOLORP3UIPROC) load("glSecondaryColorP3ui");
	glad_glSecondaryColorP3uiv = (PFNGLSECONDARYCOLORP3UIVPROC) load("glSecondaryColorP3uiv");
}
static int find_extensionsGL(void) {
	if (!get_exts()) return 0;
	(void) &has_ext;
	free_exts();
	return 1;
}

static void find_coreGL(void) {

	/* Thank you @elmindreda
	 * https://github.com/elmindreda/greg/blob/master/templates/greg.c.in#L176
	 * https://github.com/glfw/glfw/blob/master/src/context.c#L36
	 */
	int i, major, minor;

	const char* version;
	const char* prefixes[] = {
		"OpenGL ES-CM ",
		"OpenGL ES-CL ",
		"OpenGL ES ",
		NULL
	};

	version = (const char*) glGetString(GL_VERSION);
	if (!version) return;

	for (i = 0; prefixes[i]; i++) {
		const size_t length = strlen(prefixes[i]);
		if (strncmp(version, prefixes[i], length) == 0) {
			version += length;
			break;
		}
	}

	/* PR #18 */
#ifdef _MSC_VER
	sscanf_s(version, "%d.%d", &major, &minor);
#else
	sscanf(version, "%d.%d", &major, &minor);
#endif

	GLVersion.major = major; GLVersion.minor = minor;
	max_loaded_major = major; max_loaded_minor = minor;
	GLAD_GL_VERSION_1_0 = (major == 1 && minor >= 0) || major > 1;
	GLAD_GL_VERSION_1_1 = (major == 1 && minor >= 1) || major > 1;
	GLAD_GL_VERSION_1_2 = (major == 1 && minor >= 2) || major > 1;
	GLAD_GL_VERSION_1_3 = (major == 1 && minor >= 3) || major > 1;
	GLAD_GL_VERSION_1_4 = (major == 1 && minor >= 4) || major > 1;
	GLAD_GL_VERSION_1_5 = (major == 1 && minor >= 5) || major > 1;
	GLAD_GL_VERSION_2_0 = (major == 2 && minor >= 0) || major > 2;
	GLAD_GL_VERSION_2_1 = (major == 2 && minor >= 1) || major > 2;
	GLAD_GL_VERSION_3_0 = (major == 3 && minor >= 0) || major > 3;
	GLAD_GL_VERSION_3_1 = (major == 3 && minor >= 1) || major > 3;
	GLAD_GL_VERSION_3_2 = (major == 3 && minor >= 2) || major > 3;
	GLAD_GL_VERSION_3_3 = (major == 3 && minor >= 3) || major > 3;
	if (GLVersion.major > 3 || (GLVersion.major >= 3 && GLVersion.minor >= 3)) {
		max_loaded_major = 3;
		max_loaded_minor = 3;
	}
}

int gladLoadGLLoader(GLADloadproc load) {
	GLVersion.major = 0; GLVersion.minor = 0;
	glGetString = (PFNGLGETSTRINGPROC) load("glGetString");
	if (glGetString == NULL) return 0;
	if (glGetString(GL_VERSION) == NULL) return 0;
	find_coreGL();
	load_GL_VERSION_1_0(load);
	load_GL_VERSION_1_1(load);
	load_GL_VERSION_1_2(load);
	load_GL_VERSION_1_3(load);
	load_GL_VERSION_1_4(load);
	load_GL_VERSION_1_5(load);
	load_GL_VERSION_2_0(load);
	load_GL_VERSION_2_1(load);
	load_GL_VERSION_3_0(load);
	load_GL_VERSION_3_1(load);
	load_GL_VERSION_3_2(load);
	load_GL_VERSION_3_3(load);

	if (!find_extensionsGL()) return 0;
	return GLVersion.major != 0 || GLVersion.minor != 0;
}

// EOF