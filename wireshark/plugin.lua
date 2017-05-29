-- Experimental Wireshark plugin for Sauron protocol
-- Author: Przemyslaw Kopanski
-- Date: 08.05.2017

sauron_proto = Proto("sauron", "Sauron Protocol")

local sauron_fields = {
    msg_type = ProtoField.uint8("sauron.type", "Message Type", base.DEC),
    pipe_address = ProtoField.ipv6("sauron.pipe_address", "Pipe address"),
    measurement_value = ProtoField.int32("sauron.measurement_value", "Measurement value", base.DEC),
    active_period = ProtoField.uint32("sauron.active_period", "Active time", base.DEC),
    inactive_period = ProtoField.uint32("sauron.inactive_period", "Inactive time", base.DEC)
}

sauron_proto.fields = sauron_fields

local message_type_names = {
    [0] = "INIT",
    [1] = "INIT_OK",
    [2] = "INIT_LAST",
    [3] = "RUN",
    [4] = "MEASUREMENT",
    [5] = "FINISH",
    [6] = "ACK",
    [7] = "TERMINATE",
    [8] = "OneHalfInitFinish"
}

function sauron_proto.dissector(buffer, pinfo, tree)
    pinfo.cols.protocol = "SAURON"
    local offset = 0
    local subtree = tree:add(sauron_proto, buffer(), "Sauron Protocol Data")

    local msg_type_string = buffer(offset, 1)
    local msg_type_uint = msg_type_string:uint()
    subtree:add(sauron_fields.msg_type, msg_type_string):append_text(": meaning: " .. message_type_names[msg_type_uint])
    offset = offset + 1

    local pipe_address_string = buffer(offset, 16)
    subtree:add(sauron_fields.pipe_address, pipe_address_string)
    offset = offset + 16

    if (msg_type_uint == 4) then
        local measurement_value_string = buffer(offset, 4)
        subtree:add(sauron_fields.measurement_value, measurement_value_string)
    end
    offset = offset + 4

    if (msg_type_uint == 3) then
        local active_period_string = buffer(offset, 4)
        subtree:add(sauron_fields.active_period, active_period_string)
        offset = offset + 4

        local inactive_period_string = buffer(offset, 4)
        subtree:add(sauron_fields.inactive_period, inactive_period_string)
        offset = offset + 4
    end
end

-- let wireshark use protocol dissector
udp_table = DissectorTable.get("udp.port")
udp_table:add(3000,sauron_proto)
