script_github	= 'https://github.com/qwe7989199/Lyric-Importer-for-Aegisub'
script_name	= 'Import Lyric file'
script_author	= 'domo & SuJiKiNen & maz_1'
script_version 	= '1.03 mod by ema'

local zzlib = {}
local reverse = {}
local bit = require("bit")
local unpack = table.unpack or unpack
local function bitstream_init(file)
	local bs = {
		file = file,
		buf = nil,
		len = nil,
		pos = 1,
		b = 0,
		n = 0,
	}
	function bs:flushb(n)
		self.n = self.n - n
		self.b = bit.rshift(self.b,n)
	end
	function bs:getb(n)
		while self.n < n do
			if self.pos > self.len then
				self.buf = self.file:read(4096)
				self.len = self.buf:len()
				self.pos = 1
			end
			self.b = self.b + bit.lshift(self.buf:byte(self.pos),self.n)
			self.pos = self.pos + 1
			self.n = self.n + 8
		end
		local ret = bit.band(self.b,bit.lshift(1,n)-1)
		self.n = self.n - n
		self.b = bit.rshift(self.b,n)
		return ret
	end
	function bs:getv(hufftable,n)
		while self.n < n do
			if self.pos > self.len then
				self.buf = self.file:read(4096)
				self.len = self.buf:len()
				self.pos = 1
			end
			self.b = self.b + bit.lshift(self.buf:byte(self.pos),self.n)
			self.pos = self.pos + 1
			self.n = self.n + 8
		end
		local h = reverse[bit.band(self.b,255)]
		local l = reverse[bit.band(bit.rshift(self.b,8),255)]
		local v = bit.band(bit.rshift(bit.lshift(h,8)+l,16-n),bit.lshift(1,n)-1)
		local e = hufftable[v]
		local len = bit.band(e,15)
		local ret = bit.rshift(e,4)
		self.n = self.n - len
		self.b = bit.rshift(self.b,len)
		return ret
	end
	function bs:close()
		if self.file then
			self.file:close()
		end
	end
	if type(file) == "string" then
		bs.file = nil
		bs.buf = file
	else
		bs.buf = file:read(4096)
	end
	bs.len = bs.buf:len()
	return bs
end
local function hufftable_create(depths)
	local nvalues = #depths
	local nbits = 1
	local bl_count = {}
	local next_code = {}
	for i=1,nvalues do
		local d = depths[i]
		if d > nbits then
			nbits = d
		end
		bl_count[d] = (bl_count[d] or 0) + 1
	end
	local table = {}
	local code = 0
	bl_count[0] = 0
	for i=1, nbits do
		code = (code + (bl_count[i-1] or 0)) * 2
		next_code[i] = code
	end
	for i=1, nvalues do
		local len = depths[i] or 0
		if len > 0 then
			local e = (i-1)*16 + len
			local code = next_code[len]
			next_code[len] = next_code[len] + 1
			local code0 = code * 2^(nbits-len)
			local code1 = (code+1) * 2^(nbits-len)
			for j=code0,code1-1 do
				table[j] = e
			end
		end
	end
	return table,nbits
end
local function inflate_block_loop(out,bs,nlit,ndist,littable,disttable)
	local lit
	repeat
		lit = bs:getv(littable,nlit)
		if lit < 256 then
			table.insert(out,lit)
		elseif lit > 256 then
			local nbits = 0
			local size = 3
			local dist = 1
			if lit < 265 then
				size = size + lit - 257
			elseif lit < 285 then
				nbits = bit.rshift(lit-261,2)
				size = size + bit.lshift(bit.band(lit-261,3)+4,nbits)
			else
				size = 258
			end
			if nbits > 0 then
				size = size + bs:getb(nbits)
			end
			local v = bs:getv(disttable,ndist)
			if v < 4 then
				dist = dist + v
			else
				nbits = bit.rshift(v-2,1)
				dist = dist + bit.lshift(bit.band(v,1)+2,nbits)
				dist = dist + bs:getb(nbits)
			end
			local p = #out-dist+1
			while size > 0 do
				table.insert(out,out[p])
				p = p + 1
				size = size - 1
			end
		end
	until lit == 256
end
local function inflate_block_dynamic(out,bs)
	local order = { 17, 18, 19, 1, 9, 8, 10, 7, 11, 6, 12, 5, 13, 4, 14, 3, 15, 2, 16 }
	local hlit = 257 + bs:getb(5)
	local hdist = 1 + bs:getb(5)
	local hclen = 4 + bs:getb(4)
	local depths = {}
	for i=1, hclen do
		local v = bs:getb(3)
		depths[order[i]] = v
	end
	for i=hclen + 1,19 do
		depths[order[i]] = 0
	end
	local lengthtable,nlen = hufftable_create(depths)
	local i = 1
	while i <= hlit + hdist do
		local v = bs:getv(lengthtable,nlen)
		if v < 16 then
			depths[i] = v
			i = i + 1
		elseif v < 19 then
			local nbt = {2,3,7}
			local nb = nbt[v-15]
			local c = 0
			local n = 3 + bs:getb(nb)
			if v == 16 then
				c = depths[i-1]
			elseif v == 18 then
				n = n + 8
			end
			for j=1,n do
				depths[i] = c
				i = i + 1
			end
		else
			error("wrong entry in depth table for literal/length alphabet: "..v);
		end
	end
	local litdepths = {} for i=1,hlit do table.insert(litdepths,depths[i]) end
	local littable,nlit = hufftable_create(litdepths)
	local distdepths = {} for i=hlit+1,#depths do table.insert(distdepths,depths[i]) end
	local disttable,ndist = hufftable_create(distdepths)
	inflate_block_loop(out,bs,nlit,ndist,littable,disttable)
end
local function inflate_block_static(out,bs)
	local cnt = { 144, 112, 24, 8 }
	local dpt = { 8, 9, 7, 8 }
	local depths = {}
	for i=1, 4 do
		local d = dpt[i]
		for j=1,cnt[i] do
			table.insert(depths,d)
		end
	end
	local littable,nlit = hufftable_create(depths)
	depths = {}
	for i=1, 32 do
		depths[i] = 5
	end
	local disttable,ndist = hufftable_create(depths)
	inflate_block_loop(out,bs,nlit,ndist,littable,disttable)
end
local function inflate_block_uncompressed(out,bs)
	bs:flushb(bit.band(bs.n,7))
	local len = bs:getb(16)
	if bs.n > 0 then
		error("Unexpected.. should be zero remaining bits in buffer.")
	end
	local nlen = bs:getb(16)
	if bit.bxor(len,nlen) ~= 65535 then
		error("LEN and NLEN don't match")
	end
	for i=bs.pos,bs.pos+len-1 do
		table.insert(out,bs.buf:byte(i,i))
	end
	bs.pos = bs.pos + len
end
local function arraytostr(array)
	local tmp = {}
	local size = #array
	local pos = 1
	local imax = 1
	while size > 0 do
		local bsize = size>=2048 and 2048 or size
		local s = string.char(unpack(array,pos,pos+bsize-1))
		pos = pos + bsize
		size = size - bsize
		local i = 1
		while tmp[i] do
			s = tmp[i]..s
			tmp[i] = nil
			i = i + 1
		end
		if i > imax then
			imax = i
		end
		tmp[i] = s
	end
	local str = ""
	for i=1,imax do
		if tmp[i] then
			str = tmp[i]..str
		end
	end
	return str
end
local function inflate_main(bs)
	local last,type
	local output = {}
	repeat
		local block
		last = bs:getb(1)
		type = bs:getb(2)
		if type == 0 then
			inflate_block_uncompressed(output,bs)
		elseif type == 1 then
			inflate_block_static(output,bs)
		elseif type == 2 then
			inflate_block_dynamic(output,bs)
		else
			error("unsupported block type")
		end
	until last == 1
	bs:flushb(bit.band(bs.n,7))
	bs:close()
	return arraytostr(output)
end
local function inflate_zlib(bs)
	local cmf = bs.buf:byte(1)
	local flg = bs.buf:byte(2)
	if (cmf*256+flg)%31 ~= 0 then
		error("zlib header check bits are incorrect")
	end
	if bit.band(cmf,15) ~= 8 then
		error("only deflate format is supported")
	end
	if bit.rshift(cmf,4) ~= 7 then
		error("unsupported window size")
	end
	if bit.band(flg,32) ~= 0 then
		error("preset dictionary not implemented")
	end
	bs.pos=3
	return inflate_main(bs)
end
function zzlib.inflate(str)
	return inflate_zlib(bitstream_init(str))
end
for i=0,255 do
	local k=0
	for j=0,7 do
		if bit.band(i,bit.lshift(1,j)) ~= 0 then
			k = k + bit.lshift(1,7-j)
		end
	end
	reverse[i] = k
end

local Kugoo_keys = {64, 71, 97, 119, 94, 50, 116, 71, 81, 54, 49, 45, 206, 210, 110, 105}

local function round(x, dec)
		-- Check argument
	if type(x) ~= "number" or dec ~= nil and type(dec) ~= "number" then
		error("number and optional number expected", 2)
	end
		-- Return number
	if dec and dec >= 1 then
		dec = 10^math.floor(dec)
		return math.floor(x * dec + 0.5) / dec
		else
		return math.floor(x + 0.5)
	end
end

local function ass_header()
	return table.concat( {
                string.char(239, 187, 191), -- UTF8-BOM: EF BB BF
		'[Script Info]\n',
		'; This script is generated by TCAX Creator\n',
		'; Welcome to TCAX forum http://www.tcax.org\n',
		'ScriptType: v4.00+\n',
		'\n',
		'[V4+ Styles]\n',
		'Format: Name, Fontname, Fontsize, PrimaryColour, SecondaryColour, OutlineColour, BackColour, Bold, Italic, Underline, StrikeOut, ScaleX, ScaleY, Spacing, Angle, BorderStyle, Outline, Shadow, Alignment, MarginL, MarginR, MarginV, Encoding\n',
		'Style: Default,Arial,20,&H00FFFFFF,&H000000FF,&H00000000,&H00000000,0,0,0,0,100,100,0,0,1,2,2,2,10,10,10,1\n',
		'\n',
		'[Events]\n',
		'Format: Layer, Start, End, Style, Name, MarginL, MarginR, MarginV, Effect, Text\n',
	} )

end

local function to_timecode(time_ms)
	time_sec = time_ms / 1000
	h = math.floor(time_sec / 3600)
	m = math.floor(time_sec % 3600 / 60)
	s = math.floor(time_sec % 60)
	ms = math.floor( ((time_sec % 60) - math.floor(time_sec % 60)) * 100 )
	return string.format('%01d:%02d:%02d.%02d', h, m, s, ms)
end

local function to_ass_line(layer, start_time, end_time, text)
	return string.format('Dialogue: %d,%s,%s,Default,,0,0,0,,%s\n', layer, to_timecode(start_time), to_timecode(end_time), text)
end

function lyric_to_ass(input, output)
	local encoded_file = io.open(input,"rb")
		if not encoded_file then
			error("Failed to load input file")
			return false
		end


	local output_file = io.open(output, 'w+')
		if not output_file then
			error("Failed to load output file")
			return false
		end

	if string.lower(string.sub(input,-4,-1)) == ".krc" then
	--Krc decode
		local header = encoded_file:read(4)
		if header ~= "krc1" then
			error("Invalid krc file")
			return false
		end
		local temp = nil
		local decoded = io.tmpfile()
		local dbyte = encoded_file:read(1)
		while dbyte do
			temp = bit.bxor(Kugoo_keys[(encoded_file:seek()-5) % 16 + 1], string.byte(dbyte))
			decoded:write(string.char(temp))
			dbyte = encoded_file:read(1)
		end
		decoded:seek("set")
		str = zzlib.inflate(decoded:read("*a"))
		k_tag = "\\k"
	--Krc decode end
	elseif string.lower(string.sub(input,-4,-1)) == ".lrc" then
		str = encoded_file:read("*all")
	end

	encoded_file:close()

	output_file:write(ass_header())

	l={}

		--Krc to ASS
	if string.lower(string.sub(input,-4,-1)) == ".krc" then
		for t1,t2,line_lyric in string.gmatch(str,"%[(%d+),(%d+)%]([^%[]*)") do
			if (t1~=nil and t2~=nil and line_lyric~=nil) then
				ls_t=t1
				le_t=t1+t2
				k_lrc=""
				for t3,t4,syl_text in string.gmatch(line_lyric,"<(%d+),(%d+),%d+>([^<]+)") do
					kdur=round(t4/10)
					syl_text=string.gsub(syl_text," ",string.format("{%s%d} ",k_tag,"0"))
					syl_text=string.gsub(string.gsub(syl_text,string.format("{%s0} ",k_tag).."[\r\n]+",""),"[\r\n]+","")
					k_lrc=k_lrc..string.format("{%s%d}",k_tag,kdur)..syl_text
				end
				l.text = k_lrc
				l.start_time = ls_t
				l.end_time = le_t
				l.layer = 0

				output_file:write( to_ass_line(l.layer, l.start_time, l.end_time, l.text) )

			end
		end

	--Lrc to ASS
	elseif string.lower(string.sub(input,-4,-1)) == ".lrc" then
		i=1
		ls_t={}
		le_t={}
		l_lrc={}
		for st_min,st_sec,st_ms,line_lyric in string.gmatch(str,"%[(%d+):(%d+)%.(%d+)%]([^\n]+)") do
			if (st_min==nil and st_sec==nil and st_ms==nil) then
				ls_t[i]=0
			else
			ls_t[i]=st_min*60*1000+st_sec*1000+st_ms*10
			l_lrc[i]=line_lyric
			i=i+1
			end
		end
		ls_t[#ls_t+1]=3599990			--for end time of last line in lrc
		ls_t[#ls_t+2]=3599990
		for j=1,#ls_t-3 do 				--for multi language lrc
			le_t[j]=ls_t[j+1]
			if le_t[j]==ls_t[j]	then
				le_t[j]=ls_t[j+2]
			end
		l.text = l_lrc[j]
		l.start_time = ls_t[j]
		l.end_time = le_t[j]
		l.layer = 0
		output_file:write( to_ass_line(l.layer, l.start_time, l.end_time, l.text) )
		end
	else
	end
	
	output_file:close()
	return true
end
