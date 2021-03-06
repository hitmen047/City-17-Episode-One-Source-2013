#include "cbase.h"
#include "face_api.h"

using namespace std;

FaceAPI::FaceAPI()
{
	m_bFaceAPIHasCamera = true;
	m_bFaceAPIInitialized = false;
}

void FaceAPI::init() //smCoord3f *head_pos, smCoord3f *head_rot)
{
	//this.head_pos = head_pos;
	//this.head_rot = head_rot;

	/*try
	{
		// Initialize the API
		THROW_ON_ERROR(smAPIInit());
		//run();
	}
	catch (std::exception &e)
	{
		cerr << e.what() << endl;
	}*/

	//Don't use check result here, we have something special for the API.
	result = smAPIInit();
	if( result < 0 )
	{
		Warning("FaceAPI failed to initialize! Please ensure that you have followed City 17's installation instructions correctly!\n");
		m_bFaceAPIHasCamera = false;
		m_bFaceAPIInitialized = false;
	}
	else
	{
		m_bFaceAPIInitialized = true;
	}
}

/*
// Handles head-tracker head-pose callbacks
void STDCALL receiveHeadPose(void *, smHTHeadPose head_pose)
{
	
	// Make output readable
	fixed(cout);
	showpos(cout);
	cout.precision(2);

	//cout << "Head Pose: ";
	//cout << "pos(";
	//cout << head_pose.head_pos.x << ",";
	//cout << head_pose.head_pos.y << ",";
	//cout << head_pose.head_pos.z << ") ";
	//cout << "rot(";
	//cout << rad2deg(head_pose.head_rot.x_rads) << ",";
	//cout << rad2deg(head_pose.head_rot.y_rads) << ",";
	//cout << rad2deg(head_pose.head_rot.z_rads) << ") ";
	//cout << "conf " << head_pose.confidence;
	//cout << endl;

	//*head_pos = head_pose.head_pos;
	//*head_rot = head_pose.head_rot;
}
 */

// Create the first available camera detected on the system
void FaceAPI::createFirstCamera()
{
	/*
	// Register the WDM category of cameras
	THROW_ON_ERROR(smCameraRegisterCategory(SM_API_CAMERA_CATEGORY_WDM));

	// Detect cameras
	smCameraInfoList info_list;
	THROW_ON_ERROR(smCameraCreateInfoList(&info_list));

	if (info_list.num_cameras == 0)
	{
		throw std::runtime_error("No cameras were detected");
	}
	else
	{
		cout << "The followings cameras were detected: " << endl;
		for (int i=0; i<info_list.num_cameras; ++i)
		{
			char buf[1024];
			THROW_ON_ERROR(smStringWriteBuffer(info_list.info[i].category,buf,1024));
			cout << "    " << i << ". Category: " << std::string(buf);
			THROW_ON_ERROR(smStringWriteBuffer(info_list.info[i].model,buf,1024));
			cout << " Model: " << std::string(buf);
			cout << " Instance: " << info_list.info[i].instance_index << endl;
		}
	}

	// Create the first camera detected on the system
	//smCamera camera;
	THROW_ON_ERROR(smCameraCreate(info_list.info[0], 0, &camera));

	// Destroy the info list
	smCameraDestroyInfoList(&info_list);

	//return camera;
	 */

	// Register the WDM category of cameras
	result = smCameraRegisterType(SM_API_CAMERA_TYPE_WDM);
	CheckResult( "Register Camera Type" );

    // Detect cameras
    smCameraInfoList info_list;
	result = smCameraCreateInfoList(&info_list);
	CheckResult( "Camera Create Info List" );

    if (info_list.num_cameras == 0)
    {
        //throw std::runtime_error("No cameras were detected");
		Msg( "No free cameras detected for FaceAPI!\n" );
    }
    else
    {
        //cout << "The followings cameras were detected: " << endl;
		Msg( "The following cameras were detected:\n" );
        for (int i=0; i<info_list.num_cameras; ++i)
        {
            char buf[1024];
            //cout << "    " << i << ". Type: " << info_list.info[i].type;
            result = smStringWriteBuffer(info_list.info[i].model,buf,1024);
			CheckResult( "String Write Buffer" );
            //cout << " Model: " << std::string(buf);
            //cout << " Instance: " << info_list.info[i].instance_index << endl;
			Msg( "Model: %s\n", buf );
			Msg( "Instance: %i\n", info_list.info[i].instance_index );
            // Print all the possible formats for the camera
            for (int j=0; j<info_list.info[i].num_formats; j++)
            {
                smCameraVideoFormat video_format = info_list.info[i].formats[j];
                //cout << "     - Format: ";
                //cout << " res (" << video_format.res.w << "," << video_format.res.h << ")";
                //cout << " image code " << video_format.format;
                //cout << " framerate " << video_format.framerate << "(hz)";
                //cout << " upside-down? " << (video_format.is_upside_down ? "y":"n") << endl;
				Msg( "	- Camera Format:\n" );
				Msg( " res ( %i, %i )\n", video_format.res.w, video_format.res.h );
				Msg( " framerate: %.2f", video_format.framerate );
            }
        }
    }

    // Create the first camera detected on the system
    //smCameraHandle camera_handle = 0;
    result = smCameraCreate(&info_list.info[0], 
                                  0 /* Don't override any settings */, 
                                  &camera);
	CheckResult( "Camera Creation" );

    // Destroy the info list
    smCameraDestroyInfoList(&info_list);

    //return camera_handle;
}


void FaceAPI::start(headPoseCallback callback)
{
	// Get the version
    int major, minor, maint;
    result = smAPIVersion(&major, &minor, &maint);
	CheckResult( "Retrieve API Version" );
    //cout << endl << "API VERSION: " << major << "." << minor << "." << maint << "." << endl << endl;

	//smAPIInit();

    // Register the WDM category of cameras
    result = smCameraRegisterType(SM_API_CAMERA_TYPE_WDM);
	CheckResult( "Camera Type Registration" );

    const bool non_commercial_license = smAPINonCommercialLicense() == SM_API_TRUE;

    if (non_commercial_license)
    {
        //cout << "Non-Commercial License restrictions apply, see doco for details." << endl;

        // Create a new Head-Tracker engine that uses the camera
        result = smEngineCreate(SM_API_ENGINE_LATEST_HEAD_TRACKER,&engine);
		CheckResult( "Engine Creation" );
    }
    else
    {
        // Print out a list of connected cameras, and choose the first camera on the system
        createFirstCamera();

        // Create a new Head-Tracker engine that uses the camera
        result = smEngineCreateWithCamera(SM_API_ENGINE_LATEST_HEAD_TRACKER,camera,&engine);
		CheckResult( "Set Engine to Camera" );
    }

    // Check license for particular engine version (always ok for non-commercial license)
    const bool engine_licensed = smEngineIsLicensed(engine) == SM_API_OK;

    // Hook up callbacks to receive output data from engine.
    // This function will return an error if the engine is not licensed.
    if (engine_licensed)
    {
        result = smHTRegisterHeadPoseCallback(engine,0,(smHTHeadPoseCallback)callback);
		CheckResult( "Register Head Pose Callback" );
    }
    /*else
    {
        cout << "Engine is not licensed, cannot obtain any output data." << endl;
    }*/

    // Start tracking
	result = smEngineStart(engine);
	CheckResult( "Start Engine" );
}

void FaceAPI::reset()
{
	result = smEngineStop(engine);
	CheckResult( "Engine Stop (Reset)" );
	result = smEngineStart(engine);
	CheckResult( "Engine Start (Reset)" );
}

void FaceAPI::end() 
{
	// Destroy engine
	result = smEngineDestroy(&engine);
	CheckResult( "Engine Shutdown" );
}

void FaceAPI::CheckResult( const char* failedSystem )
{
	if( result < 0 )
	{
		DevMsg("FaceAPI - Failed at step: '%s'\n", failedSystem);
		m_bFaceAPIHasCamera = false;
	}
}

