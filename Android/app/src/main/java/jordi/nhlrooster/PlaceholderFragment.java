package jordi.nhlrooster;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.support.v4.app.Fragment;
import android.support.v4.widget.SwipeRefreshLayout;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AbsListView;
import android.widget.LinearLayout;
import android.widget.ListView;

import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import jordi.nhlrooster.Activities.MainActivity;

/**
 * A placeholder fragment containing a simple view.
 */
public class PlaceholderFragment extends Fragment {

    public static PlaceholderFragment newInstance(int sectionNumber) {
        PlaceholderFragment fragment = new PlaceholderFragment();
        Bundle args = new Bundle();
        args.putInt("viewId", sectionNumber);
        fragment.setArguments(args);
        return fragment;
    }

    public PlaceholderFragment() {
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_main, container, false);
        final View tempRootView = rootView;
        final int viewId = getArguments().getInt("viewId");
        MainActivity.views[viewId] = tempRootView;

        final SwipeRefreshLayout swipeView = (SwipeRefreshLayout)rootView. findViewById(R.id.swiperefresh);
        swipeView.setOnRefreshListener(new SwipeRefreshLayout.OnRefreshListener() {
            @Override
            public void onRefresh() {
                swipeView.setRefreshing(true);
                if(AppSettings.debug)
                    Log.d("test", "Refreshing");
                ( new Handler()).postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            new WebFetchRooster(getActivity(), tempRootView, "", "", false, viewId - 1).execute().get(5000, TimeUnit.MILLISECONDS);
                        }catch(InterruptedException | ExecutionException | TimeoutException e){
                            //TODO: Toast that update has failed
                        }
                        swipeView.setRefreshing(false);
                    }
                }, 100);
            }
        });
        LinearLayout rl = (LinearLayout)rootView.findViewById(R.id.layout1);
        final ListView list = (ListView)rl.findViewById(R.id.list);

        list.setOnScrollListener(new AbsListView.OnScrollListener() {
            @Override
            public void onScrollStateChanged(AbsListView view, int scrollState) {

            }

            @Override
            public void onScroll(AbsListView view, int firstVisibleItem, int visibleItemCount, int totalItemCount) {
                int topRowVerticalPosition =
                        (list == null || list.getChildCount() == 0) ?
                                0 : list.getChildAt(0).getTop();
                swipeView.setEnabled(firstVisibleItem == 0 && topRowVerticalPosition >= 0);
            }
        });

        if(MainActivity.forceRefresh) {
            MainActivity.forceRefreshCount++;
            if (MainActivity.forceRefreshCount == AppSettings.nrOfFragments) { // make sure all visible pages are being refreshed
                MainActivity.forceRefresh = false;
                MainActivity.forceRefreshCount = 0;
            }
            if(AppSettings.debug)
                Log.d("test", "Forcing network fetch");
            // notification triggered this.
        }

            new WebFetchRooster(getActivity(), rootView, "", "", !MainActivity.forceRefresh, viewId - 1).execute();
        return rootView;
    }
}