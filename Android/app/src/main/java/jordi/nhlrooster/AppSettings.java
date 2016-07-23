package jordi.nhlrooster;

public class AppSettings {
    // Our constants
    public static final boolean debug = true;
    public static final String serverIp = "178.62.227.232:8080"; //"178.62.227.232:8080";
    public static final String SENDER_ID = "1039794065372";
    public static final String PROPERTY_REG_ID = "registration_id";
    public static final String PROPERTY_APP_VERSION = "appVersion";
    public static final String PROPERTY_DEVICE_ID = "deviceId";
    public static final String PROPERTY_CHANGELOG = "changeLog";

    // Global variables
    public static int nrOfFragments = 10;
    public static String deviceId;
    public static boolean enablePushNotification = true;
    public static String savedClassname = "";
    public static boolean changeLog = false;
}
