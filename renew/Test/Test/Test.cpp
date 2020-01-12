#include <cstdio>
#include <_Time.h>
#include <GL/_OpenGL.h>
#include <GL/_NBody.h>
#include <GL/_Window.h>
#include <GL/_OpenVR.h>

Math::mat4<float> ConvertSteamVRMatrixToMatrix4(vr::HmdMatrix34_t* a)
{
	Math::mat<float, 3, 4>tp(*(Math::mat<float, 3, 4>*)a);
	Math::mat3<float>ttp(tp);
	Math::mat4<float>tttp(!ttp);
	Math::vec4<float> dr(-tp.column(3));
	dr.data[3] = 0;
	dr = (tttp, dr);
	//dr *= 2;
	dr.data[3] = 1;
	tttp.setCol(dr, 3);
	return tttp;
}

int main()
{
	vr::IVRSystem* m_pHMD;
	vr::EVRInitError eError = vr::VRInitError_None;
	m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);

	if (eError != vr::VRInitError_None)
	{
		m_pHMD = NULL;
		char buf[1024];
		printf(buf, sizeof(buf), "Unable to init VR runtime: %s",
			vr::VR_GetVRInitErrorAsEnglishDescription(eError));
		printf("VR_Init Failed");
		return false;
	}

	vr::HmdMatrix44_t mat4 = m_pHMD->GetProjectionMatrix(vr::Eye_Left, 0.1f, 30.0f);
	Math::mat4<float>ahh(*(Math::mat4<float>*) & mat4);
	ahh.print();
	mat4 = m_pHMD->GetProjectionMatrix(vr::Eye_Right, 0.1f, 30.0f);
	ahh = *(Math::mat4<float>*) & mat4;
	ahh.print();

	vr::HmdMatrix34_t mat34 = m_pHMD->GetEyeToHeadTransform(vr::Eye_Left);
	Math::mat4<float>bhh(*(Math::mat<float, 3, 4>*) & mat34);
	bhh.print();
	mat34 = m_pHMD->GetEyeToHeadTransform(vr::Eye_Right);
	bhh = *(Math::mat<float, 3, 4>*) & mat34;
	bhh.print();

	vr::TrackedDevicePose_t m_rTrackedDevicePose;



	OpenGL::OpenGLInit(4, 5);
	Window::Window::Data winPara
	{
		"NBody",
		{
			{640,360},
			true,false
		}
	};
	Window::WindowManager wm(winPara);
	OpenGL::NBody nBody(20);
	wm.init(0, &nBody);
	glfwSwapInterval(1);
	FPS fps;
	fps.refresh();
	while (!wm.close())
	{
		vr::VRCompositor()->WaitGetPoses(&m_rTrackedDevicePose, 1, NULL, 0);
		Math::mat4<float> tp;
		if (m_rTrackedDevicePose.bPoseIsValid)
		{
			tp = ConvertSteamVRMatrixToMatrix4(&m_rTrackedDevicePose.mDeviceToAbsoluteTracking);
			::printf("\r");
			for (int c0(0); c0 < 16; ++c0)::printf("%.3f ", *((float*)tp.array + c0));
		}
		wm.pullEvents();
		nBody.trans.trans = tp;
		nBody.trans.calcAns();
		nBody.trans.updated = true;

		wm.render();
		wm.swapBuffers();
		//fps.refresh();
		//fps.printFPSAndFrameTime(2, 3);
		//wm.windows[0].data.setTitle(fps.str);
	}
	vr::VR_Shutdown();
	return 1;
}