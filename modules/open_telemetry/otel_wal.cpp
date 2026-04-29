#include "otel_wal.h"

#include "core/io/file_access.h"
#include "core/io/json.h"
#include "core/os/os.h"

bool OTelWAL::open(const String &p_path) {
	_path = p_path;
	// Touch the file so it exists.
	Ref<FileAccess> f = FileAccess::open(_path, FileAccess::WRITE_READ);
	_open = f.is_valid();
	return _open;
}

void OTelWAL::close() {
	_open = false;
	_path = "";
}

bool OTelWAL::write(const String &p_signal, const String &p_id, const String &p_payload) {
	if (!_open) {
		return false;
	}
	Ref<FileAccess> f = FileAccess::open(_path, FileAccess::READ_WRITE);
	if (!f.is_valid()) {
		return false;
	}
	f->seek_end();

	Dictionary entry;
	entry["id"] = p_id;
	entry["signal"] = p_signal;
	entry["payload"] = p_payload;
	entry["ts"] = (int64_t)OS::get_singleton()->get_unix_time();
	f->store_line(JSON::stringify(entry));
	return true;
}

Vector<OTelWAL::Row> OTelWAL::read_all() {
	Vector<Row> result;
	if (!_open) {
		return result;
	}
	Ref<FileAccess> f = FileAccess::open(_path, FileAccess::READ);
	if (!f.is_valid()) {
		return result;
	}
	while (!f->eof_reached()) {
		String line = f->get_line().strip_edges();
		if (line.is_empty()) {
			continue;
		}
		Variant parsed = JSON::parse_string(line);
		if (parsed.get_type() != Variant::DICTIONARY) {
			continue;
		}
		Dictionary d = parsed;
		Row r;
		r.id = d.get("id", "").operator String();
		r.signal = d.get("signal", "").operator String();
		r.payload = d.get("payload", "").operator String();
		if (!r.id.is_empty() && !r.signal.is_empty() && !r.payload.is_empty()) {
			result.push_back(r);
		}
	}
	return result;
}

bool OTelWAL::remove(const String &p_id) {
	if (!_open) {
		return false;
	}
	Vector<Row> rows = read_all();
	Ref<FileAccess> f = FileAccess::open(_path, FileAccess::WRITE);
	if (!f.is_valid()) {
		return false;
	}
	for (int i = 0; i < rows.size(); i++) {
		if (rows[i].id == p_id) {
			continue;
		}
		Dictionary entry;
		entry["id"] = rows[i].id;
		entry["signal"] = rows[i].signal;
		entry["payload"] = rows[i].payload;
		f->store_line(JSON::stringify(entry));
	}
	return true;
}
