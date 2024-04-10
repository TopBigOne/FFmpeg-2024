package com.better.year.activity;

import static com.better.year.media.FFMediaPlayer.MEDIA_PARAM_VIDEO_DURATION;
import static com.better.year.media.FFMediaPlayer.MEDIA_PARAM_VIDEO_HEIGHT;
import static com.better.year.media.FFMediaPlayer.MEDIA_PARAM_VIDEO_WIDTH;
import static com.better.year.media.FFMediaPlayer.MSG_DECODER_DONE;
import static com.better.year.media.FFMediaPlayer.MSG_DECODER_INIT_ERROR;
import static com.better.year.media.FFMediaPlayer.MSG_DECODER_READY;
import static com.better.year.media.FFMediaPlayer.MSG_DECODING_TIME;
import static com.better.year.media.FFMediaPlayer.MSG_REQUEST_RENDER;
import static com.better.year.media.FFMediaPlayer.VIDEO_RENDER_ANWINDOW;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.SurfaceHolder;
import android.widget.SeekBar;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import com.better.year.R;
import com.better.year.media.FFMediaPlayer;
import com.better.year.media.MySurfaceView;

public class NativeMediaPlayerActivity extends AppCompatActivity {
    private static final String        TAG                     = "NativeMediaPlayerActivity";
    private static final String[]      REQUEST_PERMISSIONS     = {Manifest.permission.WRITE_EXTERNAL_STORAGE,};
    private static final int           PERMISSION_REQUEST_CODE = 1;
    private              MySurfaceView mSurfaceView            = null;
    private              FFMediaPlayer mMediaPlayer            = null;
    private              SeekBar       mSeekBar                = null;
    private              boolean       mIsTouch                = false;
    private              String        mVideoPath              = Environment.getExternalStorageDirectory().getAbsolutePath() + "/byteflow/one_piece.mp4";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_native_media_player);

        mSurfaceView = findViewById(R.id.surface_view);
        addSurfaceViewHolderCallback();

        mSeekBar = findViewById(R.id.seek_bar);
        mSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                mIsTouch = true;
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                Log.d(TAG, "onStopTrackingTouch() called with: progress = [" + seekBar.getProgress() + "]");
                if (mMediaPlayer != null) {
                    mMediaPlayer.seekToPosition(mSeekBar.getProgress());
                    mIsTouch = false;
                }

            }
        });


    }

    private void addSurfaceViewHolderCallback() {
        mSurfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder surfaceHolder) {
                mMediaPlayer = new FFMediaPlayer();
                mMediaPlayer.addEventCallback(new FFMediaPlayer.EventCallback() {
                    @Override
                    public void onPlayerEvent(int msgType, float msgValue) {
                        processPlayerEvent(msgType, msgValue);

                    }
                });

                mMediaPlayer.init(mVideoPath, VIDEO_RENDER_ANWINDOW, surfaceHolder.getSurface());

            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder surfaceHolder, int format, int w, int h) {
                Log.d(TAG, "surfaceChanged() called with: surfaceHolder = [" + surfaceHolder + "], format = [" + format + "], w = [" + w + "], h = [" + h + "]");
                mMediaPlayer.play();

            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {
                Log.d(TAG, "surfaceDestroyed() called with: surfaceHolder = [" + surfaceHolder + "]");
                mMediaPlayer.stop();
                mMediaPlayer.unInit();

            }
        });

    }

    private void processPlayerEvent(int msgType, float msgValue) {

        Log.d(TAG, "onPlayerEvent() called with: msgType = [" + msgType + "], msgValue = [" + msgValue + "]");
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                switch (msgType) {
                    case MSG_DECODER_INIT_ERROR:
                        break;
                    case MSG_DECODER_READY:
                        onDecoderReady();
                        break;
                    case MSG_DECODER_DONE:
                        break;
                    case MSG_REQUEST_RENDER:
                        break;
                    case MSG_DECODING_TIME:
                        if (!mIsTouch) mSeekBar.setProgress((int) msgValue);
                        break;
                    default:
                        break;
                }
            }
        });


    }

    @Override
    protected void onResume() {
        super.onResume();
        if (!hasPermissionsGranted(REQUEST_PERMISSIONS)) {
            ActivityCompat.requestPermissions(this, REQUEST_PERMISSIONS, PERMISSION_REQUEST_CODE);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if (requestCode == PERMISSION_REQUEST_CODE) {
            //            if (!hasPermissionsGranted(REQUEST_PERMISSIONS)) {
            //                Toast.makeText(this, "We need the permission: WRITE_EXTERNAL_STORAGE", Toast.LENGTH_SHORT).show();
            //            }
        } else {
            super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        }
    }


    @Override
    protected void onPause() {
        super.onPause();
    }

    private void onDecoderReady() {
        int videoWidth  = (int) mMediaPlayer.getMediaParams(MEDIA_PARAM_VIDEO_WIDTH);
        int videoHeight = (int) mMediaPlayer.getMediaParams(MEDIA_PARAM_VIDEO_HEIGHT);
        if (videoHeight * videoWidth != 0) mSurfaceView.setAspectRatio(videoWidth, videoHeight);

        int duration = (int) mMediaPlayer.getMediaParams(MEDIA_PARAM_VIDEO_DURATION);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            mSeekBar.setMin(0);
        }
        mSeekBar.setMax(duration);
    }


    protected boolean hasPermissionsGranted(String[] permissions) {
        for (String permission : permissions) {
            if (ActivityCompat.checkSelfPermission(this, permission) != PackageManager.PERMISSION_GRANTED) {
                return false;
            }
        }
        return true;
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

}