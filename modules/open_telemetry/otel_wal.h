#pragma once

#include "core/string/ustring.h"
#include "core/templates/vector.h"

// JSONL write-ahead log for OpenTelemetry telemetry.
// Each line: {"id":"<uuid>","signal":"<signal>","payload":"<escaped json>","ts":<unix>}
// Rows are deleted only after a successful HTTP export.
class OTelWAL {
	String _path;
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
