package jordi.nhlrooster;

import android.content.SharedPreferences;
import android.content.res.Resources;
import android.os.AsyncTask;
import android.support.v4.app.FragmentActivity;
import android.support.v4.view.ViewPager;
import android.text.Html;
import android.util.Log;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.URL;
import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.HashMap;
import java.util.List;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import jordi.nhlrooster.Activities.MainActivity;
import jordi.nhlrooster.Adapters.LessonAdapter;

public class WebFetchRooster extends AsyncTask<String, Void, Void> {
    private class Lesson {
        String title;
        String teacher;
        String location;
        Calendar starttime;
        Calendar endtime;
        String year;
        String comment;
        String change;

        public Lesson(String title, String teacher, String location, Calendar starttime, Calendar endtime, String year, String comment, String change) {
            this.starttime = starttime;
            this.endtime = endtime;
            this.title = title;
            this.teacher = teacher;
            this.location = location;
            this.year = year;
            this.comment = comment;
            this.change = change;
        }
    }
    private class Day {
        public Day(String dayStr) {
            lessons = new ArrayList<>();
            name = dayStr;
        }
        List<Lesson> lessons;
        String name;
    }

    private Calendar currentDate;
    private FragmentActivity activity;
    private View rootView;
    private String classID;
    private List<Day> wholeDay = new ArrayList<>();
    private int weekNr;
    private boolean fromCache;
    private int fetchWeekNr;
    private Calendar updateDate;
    private List<Lesson> lessonList = new ArrayList<>();
    private enum error_t {
        ERROR_NONE,
        ERROR_INVALID_DATA,
        ERROR_NODEPARTMENTORCLASS,
        ERROR_NONETWORK,
        ERROR_NONE_BUT_CACHE
    }
    private error_t errorCode = error_t.ERROR_NONE;

    public WebFetchRooster(FragmentActivity fragmentActivity, View rootView, String departmentID, String classID, boolean fromCache, int fetchWeekNr){
        this.activity = fragmentActivity;
        this.rootView = rootView;
        this.classID = classID;
        this.fromCache = fromCache;
        this.fetchWeekNr = fetchWeekNr;
    }
    @Override
    protected Void doInBackground(String... params) {
        if(AppSettings.changeLog)
            fromCache = false;
        int dayOfWeek = Calendar.getInstance().get(Calendar.DAY_OF_WEEK);
        if(dayOfWeek == Calendar.SATURDAY || dayOfWeek == Calendar.SUNDAY) // switch to next week
            fetchWeekNr++;
        URL url;
        FileInputStream cacheInputStream;
        if(AppSettings.savedClassname.isEmpty()){ // do not try a fetch when we do not have a saved classname
            errorCode = error_t.ERROR_NODEPARTMENTORCLASS;
            return null;
        }
        if(AppSettings.debug)
            Log.d("test", "From cache: " + fromCache);
        Calendar timestampCal = Calendar.getInstance();
        if(AppSettings.debug)
        Log.d("test", "Real date(unmodified) is: " + timestampCal.getTime().toString());

        if(fetchWeekNr != 0)
            timestampCal.add(Calendar.WEEK_OF_YEAR, fetchWeekNr);

        timestampCal.set(Calendar.DAY_OF_WEEK, Calendar.MONDAY);
        if(AppSettings.debug)
        Log.d("test", "Date is: " + timestampCal.getTime().toString());
        // now fetch week nr
        this.weekNr = timestampCal.get(Calendar.WEEK_OF_YEAR);

        try {
            cacheInputStream = new FileInputStream(this.activity.getApplicationContext().getFilesDir() + "/" + AppSettings.savedClassname.replace(" ", "") +  "-cache" + this.weekNr + ".xml");
        }
        catch(FileNotFoundException e){
            cacheInputStream = null;
            if(fromCache) {
                if(AppSettings.debug)
                    Log.d("test", "No cache" + this.weekNr + ", use network");
                // use network
                fromCache = false;
            }
        }
        DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
        DocumentBuilder db = null;
        try{ db = dbf.newDocumentBuilder();} catch(ParserConfigurationException e){ return null;}
        Document doc = null;
        if(fromCache) {
            try { doc = db.parse(cacheInputStream); } catch(SAXException | IOException e){ return null; }
            doc.getDocumentElement().normalize();
            NodeList nodeList = doc.getElementsByTagName("date");
            Calendar cal = Calendar.getInstance();
            cal.setTimeInMillis(Long.valueOf(nodeList.item(0).getTextContent()));
            long diff = Calendar.getInstance().getTimeInMillis() -  cal.getTimeInMillis();
            long diffDays = diff / (24 * 60 * 60 * 1000);
            if(diffDays > 1) {
                //Log.d("test", "date: " + cal.getTime().toString());
                // if cache is too old, use network fetch
                if (AppSettings.debug)
                    Log.d("test", "Cache expired, use network");
                fromCache = false;
            }
        }

        this.currentDate = (Calendar)timestampCal.clone();
        if(AppSettings.debug)
        Log.d("test", "Weeknr: " + this.weekNr);
        if(!fromCache) { // only update cache when we have the current week
            try {
<<<<<<< HEAD
                //if(AppSettings.changeLog)
                //    url = new URL("http://" + AppSettings.serverIp + "/fetchChangelog.php?deviceId=" + AppSettings.deviceId + "&classId=" + classID);
               // else
                url = new URL("http://" + AppSettings.serverIp + "/fetchWeek.php?deviceId=" + AppSettings.deviceId + "&weekNr=" + weekNr + "&classId=" + classID);
                if (AppSettings.debug)
                    Log.d("test", "url: http://" + AppSettings.serverIp + "/fetchWeek.php?deviceId=" + AppSettings.deviceId + "&weekNr=" + weekNr + "&classId=" + classID);
                if(AppSettings.changeLog){
                    try {
                        FileInputStream changeFile = new FileInputStream(this.activity.getApplicationContext().getFilesDir() + "/lastChanges.xml");
                        doc = db.parse(changeFile);
                    }catch(FileNotFoundException e){
                        return null;
                    }
                }else
                    doc = db.parse(new InputSource(url.openStream()));
=======
                if(AppSettings.changeLog)
                    url = new URL("http://" + AppSettings.serverIp + "/fetchChangelog.php?deviceId=" + AppSettings.deviceId + "&classId=" + classID);
                else
                    url = new URL("http://" + AppSettings.serverIp + "/fetchWeek.php?deviceId=" + AppSettings.deviceId + "&weekNr=" + weekNr + "&classId=" + classID);
                if (AppSettings.debug)
                    Log.d("test", "url: http://" + AppSettings.serverIp + "/fetchWeek.php?deviceId=" + AppSettings.deviceId + "&weekNr=" + weekNr + "&classId=" + classID);
                doc = db.parse(new InputSource(url.openStream()));
>>>>>>> 8f31076d2163ca7f10a25dc608e878be24b76254
                doc.getDocumentElement().normalize();
            } catch (SAXException e) {
                errorCode = error_t.ERROR_NODEPARTMENTORCLASS;
                if (AppSettings.debug)
                    Log.d("test", "server failed to reply with proper data");
                return null;
            } catch (IOException e) {
                if (AppSettings.debug)
                    Log.d("test", "Can't connect to host");
                // try cache
                try {
                    doc = db.parse(cacheInputStream);
                    doc.getDocumentElement().normalize();
                    fromCache = true;
                    if(AppSettings.debug)
                        Log.d("test", "Show a cached copy");
                    errorCode = error_t.ERROR_NONE_BUT_CACHE;
                } catch (Exception ex) {
                    if(AppSettings.debug)
                        Log.d("test", "Exception: " + ex.getMessage());
                    // no cache available, so give the error
                    errorCode = error_t.ERROR_NONETWORK;
                    return null;
                }
            }
        }
if(!AppSettings.changeLog) { // Do not modify cache when there's a changelog being fetched
    if (!fromCache) {

        // update cache
        try {
            TransformerFactory transformerFactory = TransformerFactory.newInstance();
            Transformer transformer = transformerFactory.newTransformer();
            DOMSource source = new DOMSource(doc);
            if (AppSettings.debug)
                Log.d("test", "Dir: " + this.activity.getApplicationContext().getFilesDir());
            StreamResult result = new StreamResult(new File(this.activity.getApplicationContext().getFilesDir() + "/" + AppSettings.savedClassname.replace(" ", "") + "-cache" + this.weekNr + ".xml"));
            transformer.transform(source, result);
            if (AppSettings.debug)
                Log.d("test", "Successfully created cache" + this.weekNr);

            SharedPreferences settings = activity.getSharedPreferences("Cache", 0);
            SharedPreferences.Editor editor = settings.edit();
            updateDate = Calendar.getInstance();
            editor.putLong("updateDate" + this.weekNr, updateDate.getTimeInMillis());
            editor.apply();
        } catch (Exception e) {
            if (AppSettings.debug)
                Log.d("test", "ERRRRRORRRR: " + e.getMessage());
            e.printStackTrace();
        }
    } else {
        SharedPreferences settings = activity.getSharedPreferences("Cache", 0);
        long dateMillis = settings.getLong("updateDate" + this.weekNr, 0);
        updateDate = Calendar.getInstance();
        updateDate.setTimeInMillis(dateMillis);
    }
}
        // end cache update
        DateFormat formatter = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        NodeList nodeList = doc.getElementsByTagName("lesson");
        for(int i=0;i<nodeList.getLength();i++){ // loop through days
            Node node = nodeList.item(i);
            NodeList lesson = node.getChildNodes();
            String title = lesson.item(0).getTextContent();
            String teacher = lesson.item(1).getTextContent();
            String location = lesson.item(2).getTextContent();
            if(AppSettings.debug)
            Log.d("test", "title: " + title + " - teacher: " + teacher + " - location: " + location);
            Calendar starttime = Calendar.getInstance();
            Calendar endtime;
            try {
                starttime.setTime(formatter.parse(lesson.item(3).getTextContent()));
                endtime = Calendar.getInstance();
                endtime.setTime(formatter.parse(lesson.item(4).getTextContent()));
            }catch(ParseException e){
                endtime = null; // this will never happen anyway
            }
            String year = lesson.item(5).getTextContent();
            String comment = lesson.item(6).getTextContent();
            String change = "";
            if(AppSettings.changeLog){
                change = lesson.item(7).getTextContent();
            }
            lessonList.add(new Lesson(title, teacher, location, starttime, endtime, year, comment, change));
        }
        return null;
    }

    @Override
    protected void onPostExecute(Void v) {
        if(AppSettings.debug)
        Log.d("test", "on post execute");
        ListView listView;
        Resources res;
        TextView tv = null;
        if(!AppSettings.changeLog) {
            final LinearLayout relativeLayout = (LinearLayout) rootView.findViewById(R.id.layout1);

            ProgressBar pbTemp = (ProgressBar) rootView.findViewById(R.id.swiperefresh).findViewById(R.id.loading);
            pbTemp.setVisibility(View.GONE);
            tv = (TextView) relativeLayout.findViewById(R.id.lastUpdate);
            listView = (ListView) relativeLayout.findViewById(R.id.list);
            res = relativeLayout.getResources();
        }else {
            listView = (ListView) rootView.findViewById(R.id.list);
            res = rootView.getResources();
        }
        ArrayList<HashMap<String, String>> lessonsList = new ArrayList<>();

       if(errorCode != error_t.ERROR_NONE && errorCode != error_t.ERROR_NONE_BUT_CACHE){
           HashMap<String,String> map2 = new HashMap<>();
           map2.put("title", res.getString(R.string.error));
           map2.put("date", " ");
           lessonsList.add(map2);
           HashMap<String,String> map1 = new HashMap<>();
           switch(errorCode){
               case ERROR_INVALID_DATA:
                    map1.put("title", res.getString(R.string.error_invaliddata));
                    break;
               case ERROR_NODEPARTMENTORCLASS:
                   map1.put("title", res.getString(R.string.error_nodepartmentorclass));
                   break;
               case ERROR_NONETWORK:
                   map1.put("title", res.getString(R.string.error_nonetwork));
           }
           map1.put("teacher", "");
           map1.put("time", "");
           map1.put("location", "");
           lessonsList.add(map1);
            LessonAdapter adapter=new LessonAdapter(activity, lessonsList);
            listView.setAdapter(adapter);
            adapter.notifyDataSetChanged();
            adapter.notifyDataSetInvalidated();
            listView.invalidate();
            return;
        }
        SimpleDateFormat formatter1 = new SimpleDateFormat("dd MMMM H:mm:ss");
        if(updateDate != null) {
            String dateFormatted = formatter1.format(updateDate.getTime());
            if(!AppSettings.changeLog)
                tv.setText(res.getString(R.string.updatedon) + " " + dateFormatted);
        }

        int currentDay = Calendar.getInstance().get(Calendar.DAY_OF_WEEK);
        int [] days = new int[8];

        int dayCount = 0;
        SimpleDateFormat dayDateFormatter=new SimpleDateFormat("dd MMM");
        SimpleDateFormat timeFormat = new SimpleDateFormat("HH:mm");
        SimpleDateFormat dayFormatter = new SimpleDateFormat("EEEE");
        if(AppSettings.changeLog){
            String dayStr = "";
            if (lessonList.size() >= 1) {
                for (Lesson l : lessonList) {
                    HashMap<String, String> map1 = new HashMap<>();
                    String starttime = "", endtime = "";
                    if (l.starttime != null) {
                        starttime = timeFormat.format(l.starttime.getTime());
                        String thisday = dayDateFormatter.format(l.starttime.getTime());
                        if(!thisday.equals(dayStr)) {
                            HashMap<String, String> map = new HashMap<>();
                            map.put("title", dayFormatter.format(l.starttime.getTime()));
                            map.put("date", thisday);
                            lessonsList.add(map);
                        }
                        dayStr = thisday;
                    }
                    if (l.endtime != null)
                        endtime = timeFormat.format(l.endtime.getTime());
<<<<<<< HEAD
                    String titleTemp = Html.fromHtml(l.title).toString(); // Deprecated in android N but not a solid solution for this else than an if/else statement with version checks
=======
                    String titleTemp = Html.fromHtml(l.title).toString();
>>>>>>> 8f31076d2163ca7f10a25dc608e878be24b76254
                    try {
                        titleTemp = new String(titleTemp.getBytes("ISO-8859-1"), "UTF-8");
                    } catch (UnsupportedEncodingException e) {
                        e.printStackTrace();
                    }
                    titleTemp = titleTemp.replace("&Atilde;&laquo;", "ë");
                    map1.put("title", titleTemp);
                    map1.put("teacher", l.teacher);
                    if (l.endtime == null)
                        map1.put("time", starttime);
                    else
                        map1.put("time", starttime + " - " + endtime);
                    map1.put("location", l.location);
                    if (l.endtime != null)
                        map1.put("realdate", Long.toString(l.endtime.getTimeInMillis()));
                    else {
                        Calendar tempEnd = l.starttime;
                        tempEnd.set(Calendar.HOUR_OF_DAY, 23);
                        tempEnd.set(Calendar.MINUTE, 53);
                        map1.put("realdate", Long.toString(tempEnd.getTimeInMillis()));
                    }
                    map1.put("realstartdate", Long.toString(l.starttime.getTimeInMillis()));
                    map1.put("year", l.year);
                    map1.put("change", l.change);
                    lessonsList.add(map1);
                }
            }
        }else {
            for (int day = Calendar.MONDAY; day <= Calendar.FRIDAY; day++) {
                HashMap<String, String> map = new HashMap<>();
                map.put("title", dayFormatter.format(currentDate.getTime()));
                map.put("date", dayDateFormatter.format(currentDate.getTime()));
                lessonsList.add(map);
                days[dayCount] = lessonsList.size();
                List<Lesson> lessons = getLessons(day);
                if (lessons.size() >= 1) {
                    for (Lesson l : lessons) {
                        HashMap<String, String> map1 = new HashMap<>();
                        String starttime = "", endtime = "";
                        if (l.starttime != null)
                            starttime = timeFormat.format(l.starttime.getTime());
                        if (l.endtime != null)
                            endtime = timeFormat.format(l.endtime.getTime());
<<<<<<< HEAD
                        String titleTemp = Html.fromHtml(l.title).toString(); // Deprecated in android N but not a solid solution for this else than an if/else statement with version checks
=======
                        String titleTemp = Html.fromHtml(l.title).toString();
>>>>>>> 8f31076d2163ca7f10a25dc608e878be24b76254
                        try {
                            titleTemp = new String(titleTemp.getBytes("ISO-8859-1"), "UTF-8");
                        } catch (UnsupportedEncodingException e) {
                            e.printStackTrace();
                        }
                        titleTemp = titleTemp.replace("&Atilde;&laquo;", "ë");
                        map1.put("title", titleTemp);
                        map1.put("teacher", l.teacher);
                        if (l.endtime == null)
                            map1.put("time", starttime);
                        else
                            map1.put("time", starttime + " - " + endtime);
                        map1.put("location", l.location);
                        if (l.endtime != null)
                            map1.put("realdate", Long.toString(l.endtime.getTimeInMillis()));
                        else {
                            Calendar tempEnd = l.starttime;
                            tempEnd.set(Calendar.HOUR_OF_DAY, 23);
                            tempEnd.set(Calendar.MINUTE, 53);
                            map1.put("realdate", Long.toString(tempEnd.getTimeInMillis()));
                        }
                        map1.put("realstartdate", Long.toString(l.starttime.getTimeInMillis()));
                        map1.put("year", l.year);
                        map1.put("comment", l.comment);
                        lessonsList.add(map1);
                    }
                } else {
                    // no lessons
                    HashMap<String, String> map2 = new HashMap<>();
                    map2.put("title", res.getString(R.string.error_nolessons));
                    map2.put("teacher", " ");
                    map2.put("time", "");
                    map2.put("location", "");
                    map2.put("realdate", Long.toString(currentDate.getTimeInMillis()));
                    map2.put("year", "");
                    map2.put("comment", "");
                    lessonsList.add(map2);
                }
                dayCount++;
                currentDate.add(Calendar.HOUR_OF_DAY, 24);
            }
        }
        LessonAdapter adapter=new LessonAdapter(activity, lessonsList);
        listView.setAdapter(adapter);
        //listView.smoothScrollToPosition(5);
        // only do this when current week is active, no reason to go to current day when looking at next week
        if(!AppSettings.changeLog) {
            if (this.currentDate.get(Calendar.WEEK_OF_YEAR) == Calendar.getInstance().get(Calendar.WEEK_OF_YEAR)) {
                int dayIndex = 0;
                switch (currentDay) {
                    case Calendar.MONDAY:
                        dayIndex = 0;
                        break;
                    case Calendar.TUESDAY:
                        dayIndex = 1;
                        break;
                    case Calendar.WEDNESDAY:
                        dayIndex = 2;
                        break;
                    case Calendar.THURSDAY:
                        dayIndex = 3;
                        break;
                    case Calendar.FRIDAY:
                        dayIndex = 4;
                        break;
                    case Calendar.SATURDAY:
                        dayIndex = 4;
                        break;
                    case Calendar.SUNDAY:
                        dayIndex = 4;
                        break;
                }
                if (AppSettings.debug)
                    Log.d("test", "Day: " + (days[dayIndex] - 1));
                listView.setSelection(days[dayIndex] - 1);
            }
        }else{
            SharedPreferences.Editor editor = this.activity.getSharedPreferences("Cache", 0).edit();
            editor.putBoolean(AppSettings.PROPERTY_CHANGELOG, false);
            editor.apply();
        }
        //relativeLayout.addView(listView);
        MainActivity.isRefreshing = false;
        //ViewPager mViewPager = (ViewPager) activity.findViewById(R.id.pager);
        //mViewPager.setCurrentItem(1);
        if(MainActivity.firstCall)
            MainActivity.firstCall = false;

        if(errorCode == error_t.ERROR_NONE_BUT_CACHE){
            if(AppSettings.changeLog)
                Toast.makeText(rootView.getContext(), R.string.no_changes, Toast.LENGTH_SHORT).show();
            else
                Toast.makeText(rootView.getContext(), R.string.error_nonetwork_cache, Toast.LENGTH_SHORT).show();
        }
    }

    private List<Lesson> getLessons(int day) {
        List<Lesson> temp = new ArrayList<>();
        for(Lesson l : lessonList){
            if(l.starttime.get(Calendar.DAY_OF_WEEK) == day){
                temp.add(l);
            }
        }
        return temp;
    }

    @Override
    protected void onPreExecute() {
    }

    @Override
    protected void onProgressUpdate(Void... values) {
    }
}
