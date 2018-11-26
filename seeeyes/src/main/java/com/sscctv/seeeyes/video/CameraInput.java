package com.sscctv.seeeyes.video;

import android.hardware.Camera;
import android.media.CamcorderProfile;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.io.IOException;

/**
 * Created by trlim on 2015. 12. 17..
 * <p>
 * 카메라 입력 처리 등의 공통 작업을 맡는다
 */
// Camera API는 camera2로 대체되었으나 API 17에서는 쓸 수가 없으므로 경고를 잠재운다
@SuppressWarnings("deprecation")
public abstract class CameraInput extends VideoInput {
    private static final String TAG = "CameraInput";

    private Camera _camera;
    private MediaRecorder _recorder;
    private String _outputPath;

    CameraInput(int input, SurfaceView surfaceView, Listener listener) {
        super(input, surfaceView, listener);
    }

    protected int getCameraId() {
        return getInput();
    }

    private Camera getCamera() {
        return _camera;
    }

    /**
     * 음성을 녹음해야 할지를 알려준다.
     *
     * @return true면 녹음하고 false이면 녹음하지 않음
     */
    protected boolean hasAudio() {
        return false;
    }

    @Override
    public void start(Bundle args) {
        //startCameraInput();
    }

    @Override
    public void stop() {
        stopCameraInput();
    }

    @Override
    public void restart() {
        super.restart();

        restartPreview();
    }

    //@Override
    //public void surfaceCreated(SurfaceHolder holder) {
    //    Log.i(TAG, "surfaceCreated");
    // The Surface has been created, acquire the camera and tell it where to draw.
    //startPreview();
    //}

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
//        Log.i(TAG, "Surface Changed  -  Format: " + format + " Width: " + width + " Height: " + height);

        if (getSurfaceHolder().getSurface() == null) {
            // preview surface does not exist
            return;
        }
//        stopPreview();
//        _camera.setParameters(parameters);

//        stop preview before making changes
//        stopPreview();

//        set preview size and make any resize, rotate or
//        reformatting changes here
//        start preview with new settings

        startPreview();
    }

    public void startCameraInput() {
        if (_camera != null) {
            //throw new IllegalStateException("Camera is already started");
//            Log.e(TAG, "Camera is already started!!! - " + _camera);
            //return;
            stopPreview();
            stopCameraInput();
        }

        int numCameras = Camera.getNumberOfCameras();

        if (numCameras > 0) {
            if (getCameraId() < numCameras) {
//                Log.i(TAG, "startCameraInput()");
                _camera = Camera.open(getCameraId());

                // get Camera parameters
                Camera.Parameters params = _camera.getParameters();

                for (int[] range : params.getSupportedPreviewFpsRange()) {
//                    Log.i(TAG, "Camera Fps Range: " + range[0] + ", " + range[1]);
                }

                params.setPreviewFpsRange(5000, 5000);
                params.setRecordingHint(false);
                _camera.setParameters(params);

            } else {
                Log.e(TAG, "No camera for input - " + getCameraId());
            }
        } else {
            Log.e(TAG, "No camera!");
        }
    }

    public void stopCameraInput() {
        if (_camera != null) {
//            Log.i(TAG, "stopCameraInput()");
            // 9. Call stopPreview() to stop updating the preview surface.
            //_camera.stopPreview();

            // 10. Important: Call release() to release the camera for use by other applications.
            // Applications should release the camera immediately in onPause() (and re-open() it in onResume()).
            _camera.release();
            _camera = null;
        }
    }

    public void startPreview() {
//        Log.i(TAG, "StartPreview");
        try {
            //Camera camera = getCamera();

            if (_camera != null) {
                // 5. Important: Pass a fully initialized SurfaceHolder to setPreviewDisplay(SurfaceHolder).
                // Without a surface, the camera will be unable to start the preview.

                _camera.setPreviewDisplay(getSurfaceHolder());

                // 6. Important: Call startPreview() to start updating the preview surface.
                // Preview must be started before you can take a picture.
                _camera.startPreview();
            }
        } catch (IOException exception) {
            Log.e(TAG, "Error setting camera preview", exception);
        } catch (RuntimeException exception) {
            Log.e(TAG, "Error starting camera preview", exception);
        }
    }

    public void stopPreview() {
        try {
            //Camera camera = getCamera();

            if (_camera != null) {
                _camera.stopPreview();
            }
        } catch (Exception e) {
            Log.e(TAG, "Error stopping camera preview", e);
        }
    }

    // HDMI 입력의 경우 비디오 입력이 바뀌면 restartPreview()만으로는 대응이 안되므로 중지했다 다시 시작할 수 있게한다.
    //protected void stopAndStartPreview() {
    //    stopPreview();
    //    startPreview();
    //}

    private void restartPreview() {
        getCamera().startPreview();
    }

    @Override
    public void takeSnapshot(SnapshotCallback callback) {
        Camera camera = getCamera();

        final SnapshotCallback cb = callback;
        final VideoInput self = this;

        camera.takePicture(new Camera.ShutterCallback() {
            @Override
            public void onShutter() {
                cb.onShutter();
            }
        }, new Camera.PictureCallback() {
            @Override
            public void onPictureTaken(byte[] data, Camera camera) {
                cb.onSnapshotTaken(SnapshotCallback.SNAPSHOT_RAW, data, self);
            }
        }, new Camera.PictureCallback() {
            @Override
            public void onPictureTaken(byte[] data, Camera camera) {
                cb.onSnapshotTaken(SnapshotCallback.SNAPSHOT_POSTVIEW, data, self);
            }
        }, new Camera.PictureCallback() {
            @Override
            public void onPictureTaken(byte[] data, Camera camera) {
                cb.onSnapshotTaken(SnapshotCallback.SNAPSHOT_JPEG, data, self);

                restartPreview();
            }
        });
    }

    @Override
    public boolean startRecording(String path) {
        if (!prepareVideoRecorder(path, hasAudio())) {
            _outputPath = null;
            return false;
        }

        _outputPath = path;

        _recorder.start();

        return true;
    }

    @Override
    public String stopRecording(boolean stat) {
        String path = null;

        if (_recorder != null) {
            _recorder.stop();

            releaseMediaRecorder();
            if (stat) stopCameraInput();

            path = _outputPath;
            _outputPath = null;
        }

        return path;
    }

    public boolean isRecording() {
        return _recorder != null;
    }

    private boolean prepareVideoRecorder(String path, boolean hasAudio) {
        Camera camera = getCamera();
        MediaRecorder recorder = new MediaRecorder();

        // Step 1: Unlock and set camera to MediaRecorder
        try {
            camera.unlock();
        } catch (RuntimeException e) {
            Log.e(TAG, "RuntimeException unlocking camera: " + e.getMessage());
            return false;
        }
        recorder.setCamera(camera);

        // Step 2: Set sources
        if (hasAudio) {
            recorder.setAudioSource(MediaRecorder.AudioSource.CAMCORDER);
        }
        recorder.setVideoSource(MediaRecorder.VideoSource.CAMERA);

//         Step 3: Set a CamcorderProfile (requires API Level 8 or higher)
//         음성 녹음을 선택할 수 있게 하기 위해 MediaRecorder.setProfile()의 코드를 가져와 수정함.
        CamcorderProfile profile = getCamcorderProfile();
        recorder.setOutputFormat(profile.fileFormat);
        recorder.setVideoFrameRate(profile.videoFrameRate);
//        if ((getInput() == VIDEO_INPUT_CVI) || (getInput() == VIDEO_INPUT_CVBS))
//            recorder.setVideoSize(profile.videoFrameWidth, profile.videoFrameHeight);
//        if((getInput() == VIDEO_INPUT_CVI) || (getInput() == VIDEO_INPUT_CVBS))
//            recorder.setVideoSize(profile.videoFrameWidth, getHeight());
//        else
        recorder.setVideoSize(getWidth(), getHeight());
        recorder.setVideoEncodingBitRate(profile.videoBitRate);
        recorder.setVideoEncoder(profile.videoCodec);
//        noinspection StatementWithEmptyBody
        if (profile.quality >= CamcorderProfile.QUALITY_TIME_LAPSE_LOW &&
                profile.quality <= CamcorderProfile.QUALITY_TIME_LAPSE_QVGA) {
//             Nothing needs to be done. Call to setCaptureRate() enables
//             time lapse video recording.
        } else if (hasAudio) {
            recorder.setAudioEncodingBitRate(profile.audioBitRate);
            recorder.setAudioChannels(profile.audioChannels);
            recorder.setAudioSamplingRate(profile.audioSampleRate);
            recorder.setAudioEncoder(profile.audioCodec);
        }

        // Step 4: Set output file
        recorder.setOutputFile(path);

        // Step 5: Set the preview output
        recorder.setPreviewDisplay(getSurfaceHolder().getSurface());

        _recorder = recorder;

        // Step 6: Prepare configured MediaRecorder
        try {
            recorder.prepare();
        } catch (IllegalStateException e) {
            Log.e(TAG, "IllegalStateException preparing MediaRecorder: " + e.getMessage());
            releaseMediaRecorder();
            return false;
        } catch (IOException e) {
            Log.e(TAG, "IOException preparing MediaRecorder: " + e.getMessage());
            releaseMediaRecorder();
            return false;
        }
        return true;
    }

    private void releaseMediaRecorder() {
        _recorder.reset();   // clear recorder configuration
        _recorder.release(); // release the recorder object
        _recorder = null;
        try {
            getCamera().reconnect();  // lock camera for later use
        } catch (IOException e) {
            Log.e(TAG, "IOException reconnecting camera: " + e.getMessage());
        }
    }

    public abstract int getWidth();

    public abstract int getHeight();

    public abstract float getRate();

    protected CamcorderProfile getCamcorderProfile() {
        int width = getWidth();
        int height = getHeight();
        int quality = CamcorderProfile.QUALITY_720P;
        if ((width > 1280) && (height >= 720))
            quality = CamcorderProfile.QUALITY_1080P;
        return CamcorderProfile.get(getCameraId(), quality);
    }
}
