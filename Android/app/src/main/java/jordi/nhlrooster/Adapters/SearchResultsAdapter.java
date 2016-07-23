package jordi.nhlrooster.Adapters;

import android.content.Context;
import android.database.Cursor;
import android.support.v4.widget.CursorAdapter;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import jordi.nhlrooster.R;

/**
 * Created by jordi on 21-3-15.
 */
public class SearchResultsAdapter extends CursorAdapter {
    static class ViewHolderItem {
        TextView text;
        TextView department;
        TextView teacher;
    }
    public SearchResultsAdapter(Context context, Cursor cursor) {
        super(context, cursor, false);
    }

    @Override
    public void bindView(View view, Context context, Cursor cursor) {
        TextView text;
        TextView department;
        TextView teacher;
        ViewHolderItem viewHolder = (ViewHolderItem)view.getTag();
        try {
            viewHolder.text.setText(cursor.getString(1));
            viewHolder.department.setText(cursor.getString(2));
            int type = cursor.getInt(3);
            String typeStr = "";
            switch(type){
                case 0:
                    typeStr = context.getString(R.string.teacher);
                    break;
                case 1:
                    typeStr = "";
                    break;
                case 2:
                    typeStr = context.getString(R.string.exam);
                    break;
            }
            viewHolder.teacher.setText(typeStr);
            //Log.d("test", " -> " + cursor.getString(1));
        }catch(Exception e){
            // list is empty, no worries
        }
    }

    @Override
    public View newView(Context context, Cursor cursor, ViewGroup parent) {
        TextView text;
        TextView department;
        TextView teacher;
        ViewHolderItem viewHolder = new ViewHolderItem();
        LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);

        View view = inflater.inflate(R.layout.searchresultitem, parent, false);

        viewHolder.text = (TextView) view.findViewById(R.id.item);
        viewHolder.department = (TextView) view.findViewById(R.id.department);
        viewHolder.teacher = (TextView) view.findViewById(R.id.teacher);
        view.setTag(viewHolder);
        return view;

    }

}
