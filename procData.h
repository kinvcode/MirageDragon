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

	//queue<int> job_queue; // �������

	CWinThread* cur_job_thread = NULL; // ��ǰִ�е������߳�

	bool cur_job_run = false; // ��ǰ��������ִ��
	
	//queue<ROLEJOB> job_list; // ��ǰ��ɫ�����б�

	queue<ROLEJOB> jobs_list; // ��ɫ�����б���

	time_t last_update_roles = 0; // �ϴθ��½�ɫ�б�ʱ��
	
	time_t last_update_roleid = 0; // �ϴθ��½�ɫIDʱ��
};

extern ProcessData PDATA;