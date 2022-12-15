#pragma once

class ProcessData {
public:
	DNFPROC dnf; // DNF进程信息结构

	HANDLE MSDK_HANDLE = NULL; // 易键鼠句柄

	CString cur_dir; // 当前工作目录

	CString log_file; // 日志路径

	CString screenshot_dir; // 截图存放目录

	string screenshot_file_s; // 截图文件(string)

	CString screenshot_file_c; // 截图文件(CString)

	bool play_model = false; // 辅助模式0手动 1自动

	bool queue_thread_run = false; // 队列线程是否在运行中

	bool manual_thread_run = false; // 手动线程是否在运行中

	//queue<int> job_queue; // 任务队列

	CWinThread* cur_job_thread = NULL; // 当前执行的任务线程

	bool cur_job_run = false; // 当前任务正在执行
	
	//queue<ROLEJOB> job_list; // 当前角色任务列表

	queue<ROLEJOB> jobs_list; // 角色任务列表集合

	time_t last_update_roles = 0; // 上次更新角色列表时间
	
	time_t last_update_roleid = 0; // 上次更新角色ID时间
};

extern ProcessData PDATA;