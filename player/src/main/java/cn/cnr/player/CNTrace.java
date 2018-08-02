package cn.cnr.player;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Date;
import android.annotation.SuppressLint;
import android.util.Log;

public class CNTrace {
	private final static String TAG = "ccnndebug";
	private final static boolean isDebug = true;
	private final static boolean isWF = false;
	@SuppressLint("SdCardPath")
    final static String sdcard = "/mnt/sdcard/";
    final static String logFile = "syslog.txt";
	final static File file = new File(sdcard);
	
	@SuppressWarnings("deprecation")
    static private void writeToFile(String str){
		if(!isWF)
			return;
		try {
			synchronized (file) {
				if (file.canWrite()) {
					Date curDate = new Date(System.currentTimeMillis());
					FileWriter fileWriter = new FileWriter(sdcard+logFile, true);
					fileWriter.write(curDate.getHours() + ":"+ curDate.getMinutes() + ":" + curDate.getSeconds()+ " ");
					fileWriter.write(str + "\n");
					fileWriter.flush();
					fileWriter.close();
				}
			}
		} catch (IOException e) {
				e.printStackTrace();
			}
	}
	
	private static String StackTrace(int n){
		//long id = Thread.currentThread().getId();
		String file = Thread.currentThread().getStackTrace()[4 + n].getFileName();
		int line = Thread.currentThread().getStackTrace()[4 + n].getLineNumber();
        String method = Thread.currentThread().getStackTrace()[4 + n].getMethodName();
		return file + "->" + method + " " +  ":" + line + " ";
	}
	
    static public void d(String msg, int n) {
        if (isDebug) {
            String log = StackTrace(n) + msg;
            Log.d(TAG, log);
            writeToFile(log);
        }
    }

    static public void d(String msg) {
        if (isDebug) {
            String log = StackTrace(0) + msg;
            Log.d(TAG, log);
            writeToFile(log);
        }
    }
	
	static public void i(String msg){
		if (isDebug){
			String log = StackTrace(0) + msg;   
			Log.i(TAG, log); 
			writeToFile(log);
			}
		}
	
	static public void e(String msg){
		if (isDebug){
			String log = StackTrace(0) + msg;
			Log.e(TAG, log); 
			writeToFile(log);
			}
		};
}
