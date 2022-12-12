#pragma once

class ProcessData {
public:
	DNFPROC dnf; // DNF������Ϣ�ṹ

	HANDLE MSDK_HANDLE = NULL; // �׼�����

	CString cur_dir; // ��ǰ����Ŀ¼

	CString log_file; // ��־·��

	CString screenshot_dir; // ��ͼ���Ŀ¼

	string screenshot_file_s; // ��ͼ�ļ�(string)

	CString screenshot_file_c; // ��ͼ�ļ�(CString)

	bool play_model = false; // ����ģʽ0�ֶ� 1�Զ�

	bool queue_thread_run = false; // �����߳��Ƿ���������

	bool manual_thread_run = false; // �ֶ��߳��Ƿ���������

	vector<int> job_queue; // �������

	CWinThread* cur_job_thread = NULL; // ��ǰִ�е������߳�

	bool cur_job_run = false; // ��ǰ��������ִ��

	time_t last_update_roles = 0; // �ϴθ��½�ɫ�б�ʱ��
};

extern ProcessData PDATA;