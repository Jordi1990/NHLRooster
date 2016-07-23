package jordi.nhlrooster;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.v4.app.NotificationCompat;
import android.util.Log;

import com.google.android.gms.gcm.GcmListenerService;

import java.util.Calendar;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class GcmIntentService extends GcmListenerService {
    public static final int NOTIFICATION_ID = 1;

    @Override
    public void onMessageReceived(String from, Bundle data){
        final SharedPreferences prefs = getSharedPreferences("Cache", 0);
        AppSettings.enablePushNotification = prefs.getBoolean("pref_key_push_notification", true);
        if (!data.isEmpty()) {  // has effect of unparcelling Bundle
            // Set changelog boolean to true, so application will see that it will need to fetch the changelist on next bootup
            SharedPreferences.Editor editor = prefs.edit();
            editor.putBoolean(AppSettings.PROPERTY_CHANGELOG, true);
            editor.apply();

            if(AppSettings.enablePushNotification) // bit nasty because the data still arrives, so this enable/disable has no effect on battery
                sendNotification(data);
            if(AppSettings.debug)
                Log.i("test", "Received: " + data.toString());
        }
    }

    // Put the message into a notification and post it.
    // This is just one simple example of what you might choose to do with
    // a GCM message.
    private void sendNotification(Bundle msg) {
        MainActivity.forceRefresh = true;
        Intent contentIntent = new Intent(this, MainActivity.class);
        contentIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP
                | Intent.FLAG_ACTIVITY_SINGLE_TOP);

        PendingIntent intent = PendingIntent.getActivity(this, 0, contentIntent, 0);
        NotificationCompat.Builder mBuilder =
                new NotificationCompat.Builder(this)
                        .setContentTitle(getResources().getString(R.string.notification_title))
                        .setSmallIcon(R.drawable.ic_stat_social_notifications)
                        .setLargeIcon(BitmapFactory.decodeResource(getResources(), R.mipmap.ic_launcher))
                        .setContentText(getResources().getString(R.string.notification_message).replace("??", msg.getString("classname")))
                        .setWhen(Calendar.getInstance().getTimeInMillis())
                        .setTicker(getResources().getString(R.string.notification_message).replace("??", msg.getString("classname")));
        mBuilder.setDefaults(Notification.DEFAULT_ALL);

        mBuilder.setAutoCancel(true);
        mBuilder.setContentIntent(intent);
        NotificationManager mNotificationManager =
                (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        mNotificationManager.notify(NOTIFICATION_ID, mBuilder.build());
    }
}
