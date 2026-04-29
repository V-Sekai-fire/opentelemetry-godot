#pragma once

#include "core/object/ref_counted.h"
#include "core/string/ustring.h"
#include "core/templates/vector.h"

class SQLite;

// SQLite WAL for OpenTelemetry telemetry.
// Rows persisted before HTTP export; deleted only after HTTP 200.
// journal_mode=WAL ensures append writes survive process crashes.
class OTelWAL {
	Ref<SQLite> _db;
	bool _open = false;

public:
	bool open(const String &p_path);
	void close();
	bool is_open() const { return _open; }

	bool write(const String &p_signal, const String &p_id, const String &p_payload);

	struct Row {
		String id;
		String signal;
		String payload;
	};
	Vector<Row> read_all();
	bool remove(const String &p_id);
};
