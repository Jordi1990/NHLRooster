package jordi.nhlrooster.Adapters;

/**
 * Created by root on 4-2-15.
 */

import android.app.Activity;
import android.graphics.Color;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ProgressBar;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.HashMap;

import jordi.nhlrooster.AppSettings;
import jordi.nhlrooster.R;

public class LessonAdapter extends BaseAdapter {
    static class ViewHolderItem {
        TextView day;
        TextView date;
        TextView title;
        TextView teacher;
        TextView time;
        TextView location;
        TextView comment;
        ProgressBar pb;
    }
    private Activity activity;
    private ArrayList<HashMap<String, String>> data;

    public LessonAdapter(Activity a, ArrayList<HashMap<String, String>> d) {
        activity = a;
        data=d;
    }

    public int getCount() {
        return data.size();
    }

    public Object getItem(int position) {
        return position;
    }

    public long getItemId(int position) {
        return position;
    }

    private int getPercentageLeft(Date start, Date end) {
        long now = System.currentTimeMillis();
        long s = start.getTime();
        long e = end.getTime();
        if (s >= e || now >= e) {
            return 0;
        }
        if (now <= s) {
            return 100;
        }
        return (int) ((e - now) * 100 / (e - s));
    }

    public View getView(int position, View convertView, ViewGroup parent) {
        ViewHolderItem viewHolder;
        HashMap<String, String> list = new HashMap<String, String>();
        list = data.get(position);
        if(convertView == null){
            LayoutInflater inflater = activity.getLayoutInflater();
            viewHolder = new ViewHolderItem();
            if(list.get("date") != null){
                convertView = inflater.inflate(R.layout.day_row,  parent, false);
                viewHolder.day = (TextView)convertView.findViewById(R.id.day_name);
                viewHolder.date = (TextView)convertView.findViewById(R.id.date);
            }else{
                convertView = inflater.inflate(R.layout.lesson_row, parent, false);
                viewHolder.title = (TextView)convertView.findViewById(R.id.title);
                viewHolder.teacher = (TextView)convertView.findViewById(R.id.teacher);
                viewHolder.time = (TextView)convertView.findViewById(R.id.time);
                viewHolder.location = (TextView)convertView.findViewById(R.id.location);
                viewHolder.pb = (ProgressBar)convertView.findViewById(R.id.progressbar);
                viewHolder.comment = (TextView)convertView.findViewById(R.id.comment);
            }
            convertView.setTag(viewHolder);
        }else{
            viewHolder = (ViewHolderItem)convertView.getTag();
            if(list.get("date") != null && viewHolder.day == null){
                // layout has changed, call again with null view
                return getView(position, null, parent);
            }else if(list.get("date") == null && viewHolder.title == null){
                return getView(position, null, parent);
            }
        }

        // set values
        if(list.get("date") != null){
            viewHolder.day.setText(list.get("title"));
            viewHolder.date.setText(list.get("date"));
            return convertView;
        }else{
            viewHolder.title.setText(list.get("title"));
            viewHolder.teacher.setText(list.get("teacher"));
            viewHolder.time.setText(list.get("time"));
            String location = list.get("location");
            if(list.get("year") != null) {
                if (!list.get("year").isEmpty()) {
                    location += " (" + convertView.getContext().getString(R.string.year) + " " + list.get("year") + ")";
                }
            }
            viewHolder.location.setText(location);
            if(list.get("comment") != null) {
                if (!list.get("comment").isEmpty()) {
                    viewHolder.comment.setText(list.get("comment"));
                    viewHolder.comment.setVisibility(View.VISIBLE);
                } else {
                    viewHolder.comment.setVisibility(View.GONE);
                }
            }else{
                viewHolder.comment.setVisibility(View.GONE);
            }

            convertView.setBackgroundColor(Color.WHITE);

            if(list.get("realdate") != null && !AppSettings.changeLog) {
                Calendar date = Calendar.getInstance();
                Long realDateLong = Long.parseLong(list.get("realdate"));
                date.setTimeInMillis(realDateLong);

                // first check if its the correct week
                if (date.compareTo(Calendar.getInstance()) == -1) {
                    convertView.setBackgroundColor(Color.LTGRAY);
                }
            }

            if(list.get("change") != null){
                if(list.get("change").equals("ADD"))
                    convertView.setBackgroundColor(Color.parseColor("#00ff00"));
                else if(list.get("change").equals("DEL"))
                    convertView.setBackgroundColor(Color.parseColor("#ff0000"));
            }

            if(list.get("realstartdate") != null && !AppSettings.changeLog) {
                Calendar dateStart = Calendar.getInstance();
                Long realstartdateLong = Long.parseLong(list.get("realstartdate"));
                dateStart.setTimeInMillis(realstartdateLong);
                viewHolder.pb.setVisibility(View.GONE);
                if(list.get("realdate") != null) {
                    Calendar date = Calendar.getInstance();
                    Long realDateLong = Long.parseLong(list.get("realdate"));
                    date.setTimeInMillis(realDateLong);
                    if (date.get(Calendar.WEEK_OF_YEAR) == Calendar.getInstance().get(Calendar.WEEK_OF_YEAR)) {
                        Calendar now = Calendar.getInstance();
                        if (now.getTime().after(dateStart.getTime()) && now.getTime().before(date.getTime())) {
                            viewHolder.pb.setProgress(100-getPercentageLeft(dateStart.getTime(), date.getTime()));
                            viewHolder.pb.setVisibility(View.VISIBLE);
                            //vi.setBackgroundColor(Color.GREEN);
                        }
                    }
                }
            }

            return convertView;
        }
    }
}
