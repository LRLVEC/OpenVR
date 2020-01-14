#include <cstdio>
#include <_Time.h>
#include <GL/_OpenGL.h>
#include <GL/_NBody.h>
#include <GL/_Window.h>
#include <GL/_OpenVR.h>


struct FramebufferDesc
{
	GLuint m_nDepthBufferId;
	GLuint m_nRenderTextureId;
	GLuint m_nRenderFramebufferId;
	GLuint m_nResolveTextureId;
	GLuint m_nResolveFramebufferId;
};
bool CreateFrameBuffer(int width, int height, FramebufferDesc& framebufferDesc)
{
	glGenFramebuffers(1, &framebufferDesc.m_nRenderFramebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nRenderFramebufferId);

	glGenRenderbuffers(1, &framebufferDesc.m_nDepthBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);

	glGenTextures(1, &framebufferDesc.m_nRenderTextureId);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, width, height, true);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId, 0);

	glGenFramebuffers(1, &framebufferDesc.m_nResolveFramebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nResolveFramebufferId);

	glGenTextures(1, &framebufferDesc.m_nResolveTextureId);
	glBindTexture(GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId, 0);

	// check FBO status
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)return false;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

int main()
{
	OpenGL::VR::VRDevice vrTest;
	vrTest.printInfo();
	OpenGL::VR::Trans vrTrans(&vrTest, { 0.01, 30 });
	//vrTest.getObjects();
	//printf("%d\n", vrTest.validObjects.length);
	//for (int c0(0); c0 < vrTest.validObjects.length; ++c0)
		//vrTest.objects[vrTest.validObjects[c0]].printInfo();

	//vr::TrackedDevicePose_t m_rTrackedDevicePose;


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
		//vr::VRCompositor()->WaitGetPoses(&m_rTrackedDevicePose, 1, NULL, 0);
		//Math::mat4<float> tp;
		//if (m_rTrackedDevicePose.bPoseIsValid)
		//{
		//	tp = ConvertSteamVRMatrixToMatrix4(&m_rTrackedDevicePose.mDeviceToAbsoluteTracking);
		//	::printf("\r");
		//	for (int c0(0); c0 < 16; ++c0)::printf("%.3f ", *((float*)tp.array + c0));
		//}
		wm.pullEvents();
		//nBody.trans.trans = tp;
		//nBody.trans.calcAns();
		//nBody.trans.updated = true;

		vrTrans.update();
		vrTrans.leftEye.offset.printInfo("\nLeft Eye Offset:");

		wm.render();
		wm.swapBuffers();
		//fps.refresh();
		//fps.printFPSAndFrameTime(2, 3);
		//wm.windows[0].data.setTitle(fps.str);
	}
	vr::VR_Shutdown();
	return 1;
}