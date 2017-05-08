-- Experimental Wireshark plugin for Sauron protocol
-- Author: Przemyslaw Kopanski
-- Date: 08.05.2017

sauron_proto = Proto("sauron", "Sauron Protocol")

local sauron_fields = {
    msg_type = ProtoField.int32("sauron.type", "Message Type", base.DEC),
    measurement_value = ProtoField.int32("sauron.measurement_value", "Measurement value", base.DEC)
}

sauron_proto.fields = sauron_fields

local message_type_names = {
    [0] = "INIT",
    [1] = "MEASUREMENT",
    [2] = "FINISH",
    [3] = "ACK",
    [4] = "ERROR"
}

function sauron_proto.dissector(buffer, pinfo, tree)
    pinfo.cols.protocol = "SAURON"
    local offset = 0
    local subtree = tree:add(sauron_proto, buffer(), "Sauron Protocol Data")

    local msg_type_string = buffer(offset, 4)
    local msg_type_uint = msg_type_string:uint()
    subtree:add(sauron_fields.msg_type, msg_type_string):append_text(": meaning: " .. message_type_names[msg_type_uint])
    offset = offset + 4

    local measurement_value_string = buffer(offset, 4)
    local measurement_value_uint = measurement_value_string:uint()
    subtree:add(sauron_fields.measurement_value, measurement_value_string)
    offset = offset + 4
end

-- let wireshark use protocol dissector
udp_table = DissectorTable.get("udp.port")
udp_table:add(7777,sauron_proto)
