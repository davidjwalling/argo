package com.davidjwalling.argo;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.view.KeyEvent;
import android.util.Log;
import android.widget.TextView;
import android.widget.EditText;
import java.io.File;

import com.davidjwalling.argo.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    static int maxLogLines = 2000;
    static String firstLogLine = "I1001 Argo Experimental [0.X]";
    static String newLine = "\n";
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
        setContentView(R.layout.activity_main);

        //  Initialize the TextView.

        final TextView tv = findViewById(R.id.textView);
        tv.setMaxLines(maxLogLines);
        tv.setMovementMethod(new ScrollingMovementMethod());
        tv.setText(getString(R.string.text_hint, firstLogLine));

        //  Handle EditText activity.

        final EditText et = findViewById(R.id.editText);
        et.requestFocus();
        et.setOnEditorActionListener((v, actionId, event) -> {

            //  We're only interested in Enter key press.

            if (event.getAction() != KeyEvent.ACTION_DOWN)
                return false;
            if (event.getKeyCode() != KeyEvent.KEYCODE_ENTER)
                return false;

            //  Done if nothing was entered.

            final String inp = et.getText().toString();
            if (inp.isEmpty())
                return true;

            //  Fetch the TextView content and append the input.
            //  If we were already at max-lines, remove the first.
            //  Update the TextView.

            String txt = tv.getText().toString() + newLine + inp;
            if (tv.getLineCount() >= maxLogLines)
                tv.setText(txt.substring(txt.indexOf(newLine)+1));
            else
                tv.setText(getString(R.string.text_hint, txt));

            //  Clear the edit text.

            et.setText("");
            return true;
        });

        //start();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        stop();
    }

    @Override
    protected void onStart() {
        super.onStart();
        start();
    }

    @Override
    protected void onStop() {
        super.onStop();
        stop();
    }

    public native void start();
    public native void stop();
}