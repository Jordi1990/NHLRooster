package jordi.nhlrooster.Activities;

import android.animation.ValueAnimator;
import android.app.SearchManager;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.database.MatrixCursor;
import android.graphics.Color;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.provider.Settings;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;
import android.support.v4.view.MenuItemCompat;
import android.support.v4.view.ViewPager;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.SearchView;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.GestureDetector;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.animation.AccelerateDecelerateInterpolator;
import android.widget.AdapterView;
import android.widget.AutoCompleteTextView;
import android.widget.EditText;
import android.widget.ExpandableListView;
import android.widget.LinearLayout;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GoogleApiAvailability;
import com.google.android.gms.gcm.GoogleCloudMessaging;

import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.InputSource;

import java.io.File;
import java.net.URL;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import jordi.nhlrooster.Adapters.DrawerAdapter;
import jordi.nhlrooster.Adapters.SearchResultsAdapter;
import jordi.nhlrooster.AppSettings;
import jordi.nhlrooster.DataObjects.SearchResult;
import jordi.nhlrooster.PlaceholderFragment;
import jordi.nhlrooster.R;
import jordi.nhlrooster.Services.RegistrationIntentService;
import jordi.nhlrooster.WebFetchRooster;


public class MainActivity extends AppCompatActivity {
    private static final int PLAY_SERVICES_RESOLUTION_REQUEST = 9000;
    public static boolean isRefreshing = false;
    public static boolean firstCall = true;
    GoogleCloudMessaging gcm;
    public static boolean forceRefresh = false;
    public static int forceRefreshCount = 0;
    Context context;
    public static View views[];
    SectionsPagerAdapter mSectionsPagerAdapter;
    private ExpandableListView mDrawerList;
    private android.support.v7.app.ActionBarDrawerToggle mDrawerToggle;
    RecyclerView mRecyclerView;                           // Declaring RecyclerView
    RecyclerView.Adapter mAdapter;                        // Declaring Adapter For Recycler View
    RecyclerView.LayoutManager mLayoutManager;            // Declaring Layout Manager as a linear layout manager
    DrawerLayout mDrawerLayout;                                  // Declaring DrawerLayout
    public boolean isSearchExpanded = false;
    public static volatile boolean searchExecuted = false;
    Handler handler = new Handler();
    Runnable runnable;
    private List<SearchResult> items = new ArrayList<SearchResult>();
    ViewPager mViewPager;
<<<<<<< HEAD:Android/app/src/main/java/jordi/nhlrooster/Activities/MainActivity.java
    String oldTitle, oldSubtitle;
=======
    String oldTitle;
>>>>>>> 8f31076d2163ca7f10a25dc608e878be24b76254:Android/app/src/main/java/jordi/nhlrooster/MainActivity.java
    public MainActivity getActivity(){
        return this;
    }

    public void setActionBarTitle(String title){
        getSupportActionBar().setTitle(title);
    }

    public void setActionBarSubtitle(String subTitle){
        getSupportActionBar().setSubtitle(subTitle);
    }

    @Override
    protected void onPostCreate(Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        mDrawerToggle.syncState();
        // Sync the toggle state after onRestoreInstanceState has occurred.

    }

    /**
     * @return Application's version code from the {@code PackageManager}.
     */
    private static int getAppVersion(Context context) {
        try {
            PackageInfo packageInfo = context.getPackageManager()
                    .getPackageInfo(context.getPackageName(), 0);
            return packageInfo.versionCode;
        } catch (PackageManager.NameNotFoundException e) {
            // should never happen
            throw new RuntimeException("Could not get package name: " + e);
        }
    }

    /**
     * Gets the current registration ID for application on GCM service.
     * <p>
     * If result is empty, the app needs to register.
     *
     * @return registration ID, or empty string if there is no existing
     *         registration ID.
     */
    private String getRegistrationId(Context context) {
        if(AppSettings.debug)
            Log.d("test", "getRegistrationId()");
        final SharedPreferences prefs = getGCMPreferences(context);
        String registrationId = prefs.getString(AppSettings.PROPERTY_REG_ID, "");
        if (registrationId.isEmpty()) {
            if(AppSettings.debug)
                Log.i("test", "Registration not found.");
            return "";
        }
        // Check if app was updated; if so, it must clear the registration ID
        // since the existing registration ID is not guaranteed to work with
        // the new app version.
        int registeredVersion = prefs.getInt(AppSettings.PROPERTY_APP_VERSION, Integer.MIN_VALUE);
        int currentVersion = getAppVersion(context);
        String registeredDevice = prefs.getString(AppSettings.PROPERTY_DEVICE_ID, "");

        if (registeredVersion != currentVersion || !(registeredDevice.equals(AppSettings.deviceId) )) {
            if(AppSettings.debug)
                Log.i("test", "App version changed.");

            // wipe cache when new version has been installed
            File io = getActivity().getFilesDir();
            for(File f : io.listFiles()){
                if(f.getAbsolutePath().contains("cache"))
                    f.delete();
            }
            return "";
        }
        if(AppSettings.debug)
            Log.d("test", "Already registered");
        return registrationId;
    }

    /**
     * @return Application's {@code SharedPreferences}.
     */
    private SharedPreferences getGCMPreferences(Context context) {
        // This sample app persists the registration ID in shared preferences, but
        // how you store the registration ID in your app is up to you.
        return getSharedPreferences(MainActivity.class.getSimpleName(),
                Context.MODE_PRIVATE);
    }

    /**
     * Check the device to make sure it has the Google Play Services APK. If
     * it doesn't, display a dialog that allows users to download the APK from
     * the Google Play Store or enable it in the device's system settings.
     */
    private boolean checkPlayServices() {
        GoogleApiAvailability apiAvailability = GoogleApiAvailability.getInstance();
        int resultCode = apiAvailability.isGooglePlayServicesAvailable(this);
        if (resultCode != ConnectionResult.SUCCESS) {
            if (apiAvailability.isUserResolvableError(resultCode)) {
                apiAvailability.getErrorDialog(this, resultCode, PLAY_SERVICES_RESOLUTION_REQUEST)
                        .show();
            } else {
                Log.d("test", "This device is not supported.");
                finish();
            }
            return false;
        }
        return true;
    }

    // You need to do the Play Services APK check here too.
    @Override
    protected void onResume() {
        super.onResume();
        checkPlayServices();
    }

    @Override
    protected void onPause(){
        super.onPause();
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent e) {
        if (keyCode == KeyEvent.KEYCODE_MENU) {
            if (!mDrawerLayout.isDrawerOpen(mRecyclerView)) {
                mDrawerLayout.openDrawer(mRecyclerView);
            }
            return true;
        }
        return super.onKeyDown(keyCode, e);
    }

    @Override
    public void onBackPressed() {
        if (mDrawerLayout.isDrawerOpen(mRecyclerView)) {
            mDrawerLayout.closeDrawer(mRecyclerView);
            return;
        }

<<<<<<< HEAD:Android/app/src/main/java/jordi/nhlrooster/Activities/MainActivity.java
        if(mViewPager.getVisibility() == View.GONE){
=======
        if(AppSettings.changeLog){
>>>>>>> 8f31076d2163ca7f10a25dc608e878be24b76254:Android/app/src/main/java/jordi/nhlrooster/MainActivity.java
            startNormalView();
            return;
        }

        super.onBackPressed();
    }

    public void startChangeLogView(){
<<<<<<< HEAD:Android/app/src/main/java/jordi/nhlrooster/Activities/MainActivity.java
        if(!AppSettings.changeLog) {
            AppSettings.changeLog = true;
            View searchView = findViewById(R.id.action_search);
            searchView.setVisibility(View.INVISIBLE);
            oldTitle = getSupportActionBar().getTitle().toString();
            oldSubtitle = getSupportActionBar().getSubtitle().toString();
            setActionBarTitle("Rooster wijzigingen");
            mViewPager.setVisibility(View.GONE);
            LinearLayout changeLayout = (LinearLayout) findViewById(R.id.changeLayout);

            changeLayout.setVisibility(View.VISIBLE);
            new WebFetchRooster(getActivity(), changeLayout, "", "", false, 0).execute();
        }
    }

    public void startAboutLayoutView(){
        LinearLayout aboutLayout = (LinearLayout) findViewById(R.id.aboutLayout);
        if(aboutLayout.getVisibility() == View.GONE) {
            View searchView = findViewById(R.id.action_search);
            searchView.setVisibility(View.INVISIBLE);
            oldTitle = getSupportActionBar().getTitle().toString();
            oldSubtitle = getSupportActionBar().getSubtitle().toString();
            setActionBarTitle(getString(R.string.action_about));
            setActionBarSubtitle("");
            mViewPager.setVisibility(View.GONE);
            aboutLayout.setVisibility(View.VISIBLE);
        }
=======
        oldTitle = getSupportActionBar().getTitle().toString();
        setActionBarTitle("Rooster wijzigingen");
        mViewPager.setVisibility(View.GONE);
        LinearLayout changeLayout = (LinearLayout)findViewById(R.id.changeLayout);

        changeLayout.setVisibility(View.VISIBLE);
        new WebFetchRooster(getActivity(), changeLayout, "", "", false, 0).execute();
>>>>>>> 8f31076d2163ca7f10a25dc608e878be24b76254:Android/app/src/main/java/jordi/nhlrooster/MainActivity.java
    }

    public void startNormalView(){
        setActionBarTitle(oldTitle);
<<<<<<< HEAD:Android/app/src/main/java/jordi/nhlrooster/Activities/MainActivity.java
        setActionBarSubtitle(oldSubtitle);
        View searchView = findViewById(R.id.action_search);
        searchView.setVisibility(View.VISIBLE);
        mViewPager.setVisibility(View.VISIBLE);
        LinearLayout changeLayout = (LinearLayout)findViewById(R.id.changeLayout);
        LinearLayout aboutLayout = (LinearLayout)findViewById(R.id.aboutLayout);
        aboutLayout.setVisibility(View.GONE);
=======
        mViewPager.setVisibility(View.VISIBLE);
        LinearLayout changeLayout = (LinearLayout)findViewById(R.id.changeLayout);
>>>>>>> 8f31076d2163ca7f10a25dc608e878be24b76254:Android/app/src/main/java/jordi/nhlrooster/MainActivity.java
        changeLayout.setVisibility(View.GONE);
        AppSettings.changeLog = false;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        context = getApplicationContext();

        Toolbar toolbar = (Toolbar) findViewById(R.id.tool_bar);
        setSupportActionBar(toolbar);
        //getSupportActionBar().setDisplayShowHomeEnabled(true);

        mRecyclerView = (RecyclerView) findViewById(R.id.RecyclerView); // Assigning the RecyclerView Object to the xml View

        mRecyclerView.setHasFixedSize(true);                            // Letting the system know that the list objects are of fixed size

<<<<<<< HEAD:Android/app/src/main/java/jordi/nhlrooster/Activities/MainActivity.java
        mAdapter = new DrawerAdapter(new int[] {R.string.action_home, R.string.action_changes, R.string.action_settings, R.string.action_about}, new int[] {R.drawable.ic_home_black_24dp, R.drawable.ic_notifications_black_24dp, R.drawable.ic_build_black_24dp, R.drawable.ic_info_black_24dp}, context);       // Creating the Adapter of DrawerAdapter class(which we are going to see in a bit)
=======
        mAdapter = new DrawerAdapter(new int[] {R.string.action_changes, R.string.action_settings, R.string.action_about}, new int[] {R.drawable.ic_notifications_black_36dp, R.drawable.ic_action_settings, R.drawable.ic_action_info}, context);       // Creating the Adapter of DrawerAdapter class(which we are going to see in a bit)
>>>>>>> 8f31076d2163ca7f10a25dc608e878be24b76254:Android/app/src/main/java/jordi/nhlrooster/MainActivity.java

        mRecyclerView.setAdapter(mAdapter);

        final GestureDetector mGestureDetector = new GestureDetector(MainActivity.this, new GestureDetector.SimpleOnGestureListener() {

            @Override
            public boolean onSingleTapUp(MotionEvent e) {
                return true;
            }

        });

        final Context c = this;
        mRecyclerView.addOnItemTouchListener(new RecyclerView.OnItemTouchListener() {
            @Override
            public void onRequestDisallowInterceptTouchEvent (boolean disallowIntercept){

            }
            @Override
            public boolean onInterceptTouchEvent(RecyclerView recyclerView, MotionEvent motionEvent) {
                View child = recyclerView.findChildViewUnder(motionEvent.getX(),motionEvent.getY());

                if(child!=null && mGestureDetector.onTouchEvent(motionEvent)){
                    mDrawerLayout.closeDrawers();

<<<<<<< HEAD:Android/app/src/main/java/jordi/nhlrooster/Activities/MainActivity.java
                    if (recyclerView.getChildAdapterPosition(child) == 4) {
                        /*
=======
                    if (recyclerView.getChildAdapterPosition(child) == 3) {
>>>>>>> 8f31076d2163ca7f10a25dc608e878be24b76254:Android/app/src/main/java/jordi/nhlrooster/MainActivity.java
                        // prepare the alert box
                        AlertDialog.Builder alertbox = new AlertDialog.Builder(c, R.style.DialogStyle);

                        try {
                            PackageInfo pInfo = getPackageManager().getPackageInfo(getPackageName(), PackageManager.GET_META_DATA);

                            // set the message to display
                            alertbox.setMessage(getString(R.string.about_message) + " " + pInfo.versionName + "-" + pInfo.versionCode);
                        }catch(PackageManager.NameNotFoundException e){
                            alertbox.setMessage(getString(R.string.about_message) + " Unknown");
                        }
                        alertbox.setTitle(R.string.app_name);
                        // show it
                        alertbox.show();
                        */
                        startAboutLayoutView();
                        return true;
<<<<<<< HEAD:Android/app/src/main/java/jordi/nhlrooster/Activities/MainActivity.java
                    }else if(recyclerView.getChildAdapterPosition(child) == 3){
                        Intent settingsIntent = new Intent(MainActivity.this, SettingsActivity.class);
                        MainActivity.this.startActivity(settingsIntent);
                    }else if(recyclerView.getChildAdapterPosition(child) == 2){
                        startChangeLogView();
                    }else if(recyclerView.getChildAdapterPosition(child) == 1) {
                        startNormalView();
=======
                    }else if(recyclerView.getChildAdapterPosition(child) == 2){
                        Intent settingsIntent = new Intent(MainActivity.this, SettingsActivity.class);
                        MainActivity.this.startActivity(settingsIntent);
                    }else if(recyclerView.getChildAdapterPosition(child) == 1){
                        AppSettings.changeLog = true;
                        startChangeLogView();
>>>>>>> 8f31076d2163ca7f10a25dc608e878be24b76254:Android/app/src/main/java/jordi/nhlrooster/MainActivity.java
                    }
                    return true;

                }

                return false;
            }

            @Override
            public void onTouchEvent(RecyclerView recyclerView, MotionEvent motionEvent) {

            }
        });

        mLayoutManager = new LinearLayoutManager(this);                 // Creating a layout Manager

        mRecyclerView.setLayoutManager(mLayoutManager);                 // Setting the layout Manager


        mDrawerLayout = (DrawerLayout) findViewById(R.id.drawer_layout);        // Drawer object Assigned to the view
        mDrawerToggle = new ActionBarDrawerToggle(this, mDrawerLayout,toolbar,R.string.navigation_drawer_open,R.string.navigation_drawer_close) {

            @Override
            public void onDrawerOpened(View drawerView) {
                super.onDrawerOpened(drawerView);
                // code here will execute once the drawer is opened( As I dont want anything happened whe drawer is
                // open I am not going to put anything here)
            }

            @Override
            public void onDrawerClosed(View drawerView) {
                super.onDrawerClosed(drawerView);
                // Code here will execute once drawer is closed
            }
        };
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        getSupportActionBar().setHomeButtonEnabled(true);

        mDrawerLayout.addDrawerListener(mDrawerToggle);
        final FragmentActivity tempActivity1 = this;
        SharedPreferences settings = getSharedPreferences("Cache", 0);
        AppSettings.savedClassname = settings.getString("class", "");
        if(AppSettings.savedClassname != "") {
            getSupportActionBar().setSubtitle(AppSettings.savedClassname);
            int dayOfWeek = Calendar.getInstance().get(Calendar.DAY_OF_WEEK);
            if(!(dayOfWeek == Calendar.SATURDAY || dayOfWeek == Calendar.SUNDAY)) // switch to next week
                getSupportActionBar().setTitle("Week " + Calendar.getInstance().get(Calendar.WEEK_OF_YEAR));
            else {
                Calendar tempCalendar = Calendar.getInstance(); // we are one week ahead
                tempCalendar.add(Calendar.WEEK_OF_YEAR, 1);
                getSupportActionBar().setTitle("Week " + tempCalendar.get(Calendar.WEEK_OF_YEAR));
            }
        }else{
            getSupportActionBar().setTitle(R.string.app_name);
        }
        getSupportActionBar().setDisplayShowTitleEnabled(true);


        AppSettings.deviceId = Settings.Secure.getString(getContentResolver(), Settings.Secure.ANDROID_ID);
        // read settings
        AppSettings.changeLog = settings.getBoolean(AppSettings.PROPERTY_CHANGELOG, false);

        if(AppSettings.debug)
            Log.d("test", "Changelog: " + AppSettings.changeLog);
        AppSettings.enablePushNotification = settings.getBoolean("pref_key_push_notification", true);
        if(AppSettings.debug)
            Log.d("test", "Push notifications: " + ((AppSettings.enablePushNotification)?"ENABLED":"DISABLED"));
        // 3 tot 20
        String nrOfFragmentsStr = settings.getString("pref_key_nr_of_fragments","10");
        try {
            int fragments = Integer.parseInt(nrOfFragmentsStr);
            AppSettings.nrOfFragments = Math.max(3, Math.min(20, fragments));
        }
        catch(Exception e) {
        }

        views = new View[AppSettings.nrOfFragments];
        if (checkPlayServices()) {
            gcm = GoogleCloudMessaging.getInstance(this);
            String regid = getRegistrationId(context);

           if (regid.isEmpty()) {
               Intent intent = new Intent(this, RegistrationIntentService.class);
               startService(intent);
          }
            if(AppSettings.debug)
                Log.d("token", regid);
        } else {
            if(AppSettings.debug)
                Log.i("test", "No valid Google Play Services APK found.");
        }

        // Create the adapter that will return a fragment for each of the three
        // primary sections of the activity.
        mSectionsPagerAdapter = new SectionsPagerAdapter(getSupportFragmentManager());

        // Set up the ViewPager with the sections adapter.
        mViewPager = (ViewPager) findViewById(R.id.pager);

        mViewPager.setAdapter(mSectionsPagerAdapter);

        if(AppSettings.changeLog){
            startChangeLogView();
        }

        //setupDrawer();
        final FragmentActivity tempActivity = this;

        mViewPager.setCurrentItem(1);

        toolbar.setNavigationOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(isSearchExpanded) {
                    collapseAndAnimateSearch();
                }else{
                    mDrawerLayout.openDrawer(mRecyclerView);
                }
            }
        });
        mViewPager.addOnPageChangeListener (new ViewPager.OnPageChangeListener(){
            @Override
            public void onPageScrolled(int i, float v, int i2) {
                //Log.d("test", "onPageScrolled " + i);
            }

            @Override
            public void onPageSelected(int i) {
                Calendar cal = Calendar.getInstance();
                int dayOfWeek = cal.get(Calendar.DAY_OF_WEEK);
                if(dayOfWeek == Calendar.SATURDAY || dayOfWeek == Calendar.SUNDAY)
                    cal.add(Calendar.WEEK_OF_YEAR, i);
                else
                    cal.add(Calendar.WEEK_OF_YEAR, i-1);

                setActionBarTitle("Week " + cal.get(Calendar.WEEK_OF_YEAR));
                setActionBarSubtitle(AppSettings.savedClassname);
                if(AppSettings.debug)
                    Log.d("test", "onPageSelected " + i);
            }

            @Override
            public void onPageScrollStateChanged(int i) {

            }
        });
    }

    private void collapseAndAnimateSearch(){
        SearchView searchView = (SearchView)findViewById(R.id.action_search);
        searchView.setQuery("", false);
        mDrawerLayout.setDrawerLockMode(DrawerLayout.LOCK_MODE_UNLOCKED);
        getSupportActionBar().setDisplayShowTitleEnabled(true);
        toggleActionBarIcon(ActionDrawableState.BURGER, mDrawerToggle, true);
        //SearchView searchView = (SearchView)MenuItemCompat.getActionView(menuItem);
        isSearchExpanded = false;
        searchView.setIconified(true);
    }

    private void expandAndAnimateSearch(final SearchView searchView){
        searchView.setIconified(false);
        mDrawerLayout.setDrawerLockMode(DrawerLayout.LOCK_MODE_LOCKED_CLOSED);
        getSupportActionBar().setDisplayShowTitleEnabled(false);
        toggleActionBarIcon(ActionDrawableState.ARROW, mDrawerToggle, true);
        //searchAutoCompleteTextView.dismissDropDown();
        isSearchExpanded = true;
    }

    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main, menu);
        MenuItem menuItem = menu.findItem(R.id.action_search);
        final SearchView searchView = (SearchView)MenuItemCompat.getActionView(menuItem);
        searchView.setQueryHint(getString(R.string.searchQueryHint));
        ((EditText)searchView.findViewById(android.support.v7.appcompat.R.id.search_src_text)).setHintTextColor(Color.LTGRAY);

        View view = searchView.findViewById(android.support.v7.appcompat.R.id.search_badge);
        view.setBackgroundColor(Color.WHITE);

        // tutorial search
        SharedPreferences settings = getSharedPreferences("Cache", 0);
        boolean searchingLearned = settings.getBoolean("searchingLearned", false);
        if(!searchingLearned){
            // show them how to search
            SharedPreferences.Editor editor = settings.edit();
            editor.putBoolean("searchingLearned", true);
            editor.apply();
            expandAndAnimateSearch(searchView);
        }
        // end tutorial

        final AutoCompleteTextView searchAutoCompleteTextView = (AutoCompleteTextView) searchView.findViewById(android.support.v7.appcompat.R.id.search_src_text);
        searchAutoCompleteTextView.setThreshold(1);

        searchAutoCompleteTextView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                collapseAndAnimateSearch();
                MatrixCursor cursor = (MatrixCursor)parent.getAdapter().getItem(position);
                SharedPreferences settings = getSharedPreferences("Cache", 0);
                SharedPreferences.Editor editor = settings.edit();
                editor.putString("class", cursor.getString(1));
                AppSettings.savedClassname = cursor.getString(1);
                editor.apply();
                mViewPager.setCurrentItem(1);
                for(int i=0;i<AppSettings.nrOfFragments;i++) {
                    if(views[i] != null) { // only fetch the fragments which are preloaded, should be 3
                        new WebFetchRooster(getActivity(), views[i], cursor.getString(2), cursor.getString(4), false, i - 1).execute();
                        if(AppSettings.debug)
                            Log.d("test", "Fetch " + i);
                    }
                    //new WebFetchRooster(tempActivity, view2, WebFetchDepartment.groupData.get(groupPosition).get("c"), WebFetchDepartment.childData.get(groupPosition).get(childPosition).get("c"), false, 0).execute();
                    //new WebFetchRooster(tempActivity, view3, WebFetchDepartment.groupData.get(groupPosition).get("c"), WebFetchDepartment.childData.get(groupPosition).get(childPosition).get("c"), false, 1).execute();
                }
                //new WebFetchRooster(tempActivity, v.getRootView(), WebFetchDepartment.groupData.get(groupPosition).get("c"), WebFetchDepartment.childData.get(groupPosition).get(childPosition).get("c").replace(' ', '_'), false, 0).execute();
                setActionBarTitle("Week " + Calendar.getInstance().get(Calendar.WEEK_OF_YEAR));
                setActionBarSubtitle( AppSettings.savedClassname);
            }
        });

        searchView.setOnCloseListener(new SearchView.OnCloseListener() {
            @Override
            public boolean onClose() {
                if (isSearchExpanded) {
                    getSupportActionBar().setDisplayShowTitleEnabled(true);
                    toggleActionBarIcon(ActionDrawableState.BURGER, mDrawerToggle, true);
                    mDrawerLayout.setDrawerLockMode(DrawerLayout.LOCK_MODE_UNLOCKED);
                    isSearchExpanded = false;
                }
                return false;
            }
        });
        searchView.setMaxWidth(Integer.MAX_VALUE);
        searchView.setOnSearchClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mDrawerLayout.setDrawerLockMode(DrawerLayout.LOCK_MODE_LOCKED_CLOSED);
                getSupportActionBar().setDisplayShowTitleEnabled(false);
                toggleActionBarIcon(ActionDrawableState.ARROW, mDrawerToggle, true);
                searchAutoCompleteTextView.dismissDropDown();
                isSearchExpanded = true;
            }
        });

        SearchManager manager = (SearchManager) getSystemService(Context.SEARCH_SERVICE);

        searchView.setSearchableInfo(manager.getSearchableInfo(getComponentName()));

        searchView.setOnQueryTextListener(new SearchView.OnQueryTextListener() {

            @Override
            public boolean onQueryTextSubmit(String s) {
                //Log.d("test", "SUBMIT");
                return false;
            }

            @Override
            public boolean onQueryTextChange(final String query) {
                if(searchExecuted){
                    handler.removeCallbacks(runnable); // This will prevent spamming the server
                }
                if(query.length() < 1) {
                    searchAutoCompleteTextView.dismissDropDown();
                    return false;
                }
                //loadHistory(query, searchView, searchAutoCompleteTextView);
                runnable = new Runnable(){
                    @Override
                    public void run() {
                        loadHistory(query, searchView, searchAutoCompleteTextView);
                        searchExecuted = false;
                    }
                };
                searchExecuted = true;
                handler.postDelayed(runnable, 200);
                return true;

            }

        });

        return true;
    }


    private void loadHistory(final String query, final SearchView searchView, final AutoCompleteTextView searchAutoCompleteTextView)
    {
        final Context tempC = this;
        items.clear();
        new AsyncTask() {

            @Override
            protected Object doInBackground(Object[] params) {
                String msg = "";
                URL url = null;
                try {
                    if(AppSettings.debug)
                        Log.d("test", "Search url: " + "http://"+AppSettings.serverIp+"/search.php?q="+query);
                    url = new URL("http://" + AppSettings.serverIp + "/search.php?q=" + URLEncoder.encode(query, "UTF-8"));
                    DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
                    DocumentBuilder db = dbf.newDocumentBuilder();
                    Document doc = db.parse(new InputSource(url.openStream()));

                    doc.getDocumentElement().normalize();
                    NodeList nodeList = doc.getElementsByTagName("result");

                    for (int i = 0; i < nodeList.getLength(); i++) {
                        Node node = nodeList.item(i);
                        NodeList children = node.getChildNodes();
                        int id = Integer.valueOf(children.item(0).getTextContent());
                        String name = children.item(1).getTextContent();
                        String department = children.item(2).getTextContent();
                        int type = Integer.valueOf(children.item(3).getTextContent());
                        items.add(new SearchResult(name,department,type, id));
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }
                return msg;
            }

            @Override
            protected void onPostExecute(Object msg) {
                // Cursor
                String[] columns = new String[]{"_id", "name", "department", "type", "classId"};
                Object[] temp = new Object[]{0, "default", "default", 0, 0};

                MatrixCursor cursor = new MatrixCursor(columns);
                for(int i = 0; i < items.size(); i++) {
                    temp[0] = i;
                    temp[1] = items.get(i).name;
                    temp[2] = items.get(i).department;
                    temp[3] = items.get(i).type;
                    temp[4] = items.get(i).id;
                    cursor.addRow(temp);
                }
                // SearchView
                SearchManager manager = (SearchManager) getSystemService(Context.SEARCH_SERVICE);
                SearchResultsAdapter adapter = new SearchResultsAdapter(tempC, cursor);
                searchView.setSuggestionsAdapter(adapter);

                searchView.getSuggestionsAdapter().notifyDataSetInvalidated();
                searchView.getSuggestionsAdapter().notifyDataSetChanged();
                searchView.invalidate();

               // if(items.size() > 0)
                //    searchAutoCompleteTextView.showDropDown();
               // else
               //     searchAutoCompleteTextView.dismissDropDown();
            }
        }.execute(null, null, null);

    }

    //region toggleActionBarIcon
    private enum ActionDrawableState{
        BURGER, ARROW
    }
    private void toggleActionBarIcon(final ActionDrawableState state, final ActionBarDrawerToggle toggle, boolean animate) {
        if (animate) {
            float start = state == ActionDrawableState.BURGER ? 1.0f : 0f;
            float end = Math.abs(start - 1);
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB) {
                ValueAnimator offsetAnimator = ValueAnimator.ofFloat(start, end);
                offsetAnimator.setDuration(300);
                offsetAnimator.setInterpolator(new AccelerateDecelerateInterpolator());
                offsetAnimator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
                    @Override
                    public void onAnimationUpdate(ValueAnimator animation) {
                        float offset = (Float) animation.getAnimatedValue();
                        toggle.onDrawerSlide(null, offset);
                    }
                });
                //toolbarHomeButtonAnimating = true;
                offsetAnimator.start();
            }
        } else {
            if (state == ActionDrawableState.BURGER) {
                toggle.onDrawerClosed(null);
            } else {
                toggle.onDrawerOpened(null);
            }
        }
    }
    //endregion

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.action_search:
                //mSearchView.setIconified(false);
                return true;
        }

        return super.onOptionsItemSelected(item);
    }


    /**
     * A {@link android.support.v4.app.FragmentPagerAdapter} that returns a fragment corresponding to
     * one of the sections/tabs/pages.
     */
    public class SectionsPagerAdapter extends FragmentPagerAdapter {

        public SectionsPagerAdapter(FragmentManager fm) {
            super(fm);
        }

        @Override
        public int getItemPosition(Object object) {
            return POSITION_NONE;
        }

        @Override
        public Fragment getItem(int position) {
            // getItem is called to instantiate the fragment for the given page.
            // Return a PlaceholderFragment (defined as a static inner class below).
            if(AppSettings.debug)
                Log.d("test", "new instance " + (position));
            return PlaceholderFragment.newInstance(position);
        }

        @Override
        public int getCount(){
            return AppSettings.nrOfFragments;
        }
    }

}
