package com.example.music;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.example.project_music.Music;

import android.R;
import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.Cursor;
import android.os.Bundle;
import android.provider.MediaStore.Audio.Media;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.AdapterView.OnItemClickListener;

public class MainActivity extends Activity implements OnClickListener {
	List<Object> musiclists = new ArrayList<Object>();
	List<Map<String, Object>> list = new ArrayList<Map<String, Object>>();
	Button play_pause, stop, onplay, downplay, close, exit;
	ActivityReceiver activityReceiver;
	public static final String CTL_ACTION = "org.crazyit.action.CTL_ACTION";
	public static final String UPDATE_ACTION = "org.crazyit.action.UPDATE_ACTION";
	Intent intentservice;
	// �������ֵĲ���״̬ ��0X11 ����ֹͣ ��0x12������,0x13������ͣ
	int status = 0x11;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		UIinit();
		logic();
		musicList();
		activityReceiver = new ActivityReceiver();
		IntentFilter filter = new IntentFilter();
		filter.addAction(UPDATE_ACTION);
		registerReceiver(activityReceiver, filter);
		intentservice = new Intent(this, MusicService.class);
		startService(intentservice);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	public void UIinit() {
		play_pause = (Button) this.findViewById(R.id.);
		stop = (Button) this.findViewById(R.id.stop);
		onplay = (Button) this.findViewById(R.id.onplay);
		close = (Button) this.findViewById(R.id.close);
		exit = (Button) this.findViewById(R.id.exit);
		downplay = (Button) this.findViewById(R.id.downplay);
	}

	public void logic() {
		play_pause.setOnClickListener(this);
		stop.setOnClickListener(this);
		onplay.setOnClickListener(this);
		downplay.setOnClickListener(this);
		close.setOnClickListener(this);
		exit.setOnClickListener(this);
	}

	@Override
	public void onClick(View source) {
		Intent intent = new Intent(CTL_ACTION);
		switch (source.getId()) {
		case R.id.play_pause: {
			intent.putExtra("control", 1);
			break;
		}
		case R.id.stop: {
			intent.putExtra("control", 2);
			break;
		}
		case R.id.onplay: {
			intent.putExtra("control", 3);
			break;
		}
		case R.id.downplay: {
			intent.putExtra("control", 4);
			break;
		}
		case R.id.close: {
			this.finish();
			break;
		}
		case R.id.exit: {
			stopService(intentservice);
			this.finish();
			break;
		}
		}
		sendBroadcast(intent);

	}

	
	public class ActivityReceiver extends BroadcastReceiver {

		@Override
		public void onReceive(Context context, Intent intent) {
			// ��ȡIntent�е�update��Ϣ��update������״̬
		 	int update = intent.getIntExtra("update", -1);
			switch (update) {
			case 0x11: {
				play_pause.setText("����");
				status = 0x11;
				break;
			}

			// ����ϵͳ���벥��״̬
			case 0x12: {
				// ����״̬������ʹ�ð�ť
				play_pause.setText("��ͣ");
				// ���õ�ǰ״̬
				status = 0x12;
				break;
			}
			// ����ϵͳ������ͣ״̬
			case 0x13: {
				play_pause.setText("����");
				status = 0x13;
				break;
			}
			}
		}

	}

	/* �����б� */
	public void musicList() {
		// ȡ��ָ��λ�õ��ļ�������ʾ�������б�
		String[] music = new String[] { Media._ID, Media.DISPLAY_NAME,
				Media.TITLE, Media.DURATION, Media.ARTIST, Media.DATA };
		Cursor cursor = getContentResolver().query(Media.EXTERNAL_CONTENT_URI,
				music, null, null, null);
		while (cursor.moveToNext()) {
			Music temp = new Music();
			temp.setFilename(cursor.getString(1));
			temp.setTitle(cursor.getString(2));
			temp.setDuration(cursor.getInt(3));
			temp.setArtist(cursor.getString(4));
			temp.setData(cursor.getString(5));
			musiclists.add(temp);

			Map<String, Object> map = new HashMap<String, Object>();
			map.put("name", cursor.getString(1));
			map.put("artist", cursor.getString(4));
			list.add(map);
		}

		ListView listview = (ListView) findViewById(R.id.musics);
		SimpleAdapter adapter = new SimpleAdapter(this, list,
				R.layout.musicsshow, new String[] { "name", "artist" },
				new int[] { R.id.name, R.id.artist });
		listview.setAdapter(adapter);
		listview.setOnItemClickListener(new OnItemClickListener() {

			@Override
			public void onItemClick(AdapterView<?> parent, View view,
					int current, long id) {

				Intent intent=new Intent(CTL_ACTION);
				intent.putExtra("control", 5);
				intent.putExtra("current", current);
				sendBroadcast(intent);
			}
		});
	}
}