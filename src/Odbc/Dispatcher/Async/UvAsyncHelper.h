#pragma once


class CQuery;
class CIoWorker;
struct uv_query_async_t : public uv_async_t
{
	std::shared_ptr< CQuery >	pQuery;
	CIoWorker*					pIoWorker;
};