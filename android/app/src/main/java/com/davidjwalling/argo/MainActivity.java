package com.davidjwalling.argo;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;
import java.io.File;

import com.davidjwalling.argo.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'argo' library on application startup.
    static {
        try {
            System.loadLibrary("argo");
        } catch (UnsatisfiedLinkError e) {
            Log.e("MainActivity", "Failed to load argo library", e);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        ActivityMainBinding binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // Example of a call to a native method
        TextView tv = binding.sampleText;
        tv.setText(stringFromJNI());
    }

    /**
     * A native method that is implemented by the 'argo' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}