param(
    [Parameter(Mandatory)] [ValidateSet('itemtype', 'magictype', 'monstertype', 'itemaddition', 'point_allot', 'goods')] [string]$Table,
    [Parameter(Mandatory)] [string]$Client,
    [string]$Out = ''
)
$ErrorActionPreference = 'Stop'
if (-not $Out) { $Out = "cq_$Table.sql" }
$latin1 = [Text.Encoding]::GetEncoding(28591)

Add-Type -TypeDefinition @'
public static class SfkClientDat
{
    public static byte[] Decrypt(byte[] b)
    {
        uint seed = 0x2537;
        byte[] key = new byte[128];
        for (int i = 0; i < 128; i++) { seed = seed * 214013u + 2531011u; key[i] = (byte)(((seed >> 16) & 0x7FFF) % 256); }
        byte[] o = new byte[b.Length];
        for (int i = 0; i < b.Length; i++)
        {
            int a = (key[i % 128] ^ b[i]) & 0xFF;
            int s = i % 8;
            o[i] = (byte)(((a << (8 - s)) | (a >> s)) & 0xFF);
        }
        return o;
    }
}
'@

function Read-ClientFile([string]$name) {
    $path = Join-Path $Client "ini\$name"
    if (-not (Test-Path -LiteralPath $path)) { throw "missing: $path" }
    return ,([IO.File]::ReadAllBytes($path))
}
function Get-SqlText([string]$s, [int]$max) {
    if ($s.Length -gt $max) { $s = $s.Substring(0, $max) }
    return "'" + $s.Replace('\', '\\').Replace("'", "''") + "'"
}

$columns = $null
$rows = New-Object System.Collections.Generic.List[string]
$skipped = 0

switch ($Table) {
    'itemtype' {
        $columns = 'id', 'name', 'req_profession', 'req_weaponskill', 'req_level', 'req_sex', 'req_force', 'req_speed',
                   'req_health', 'req_soul', 'monopoly', 'weight', 'price', 'id_action', 'attack_max', 'attack_min',
                   'defense', 'dexterity', 'dodge', 'life', 'mana', 'amount', 'amount_limit', 'ident', 'gem1', 'gem2',
                   'magic1', 'magic2', 'magic3', 'magic_atk', 'magic_def', 'atk_range', 'atk_speed', 'fray_mode',
                   'repair_mode', 'type_mask', 'emoney_price'
        $text = $latin1.GetString([SfkClientDat]::Decrypt((Read-ClientFile 'itemtype.dat')))
        $lines = $text -split "`r?`n"
        if ($lines[0].Trim() -notmatch '^Amount=(\d+)$') { throw "itemtype.dat: no Amount line (wrong key?)" }
        $amount = [int]$Matches[1]
        $seen = @{}
        foreach ($line in ($lines | Select-Object -Skip 1)) {
            $t = $line.Trim()
            if (-not $t) { continue }
            $f = $t -split ' +'
            if ($f.Count -lt $columns.Count) { throw "itemtype.dat: a line of $($f.Count) fields: $t" }
            if ($seen.ContainsKey($f[0])) { $skipped++; continue }
            $seen[$f[0]] = $true
            $v = for ($c = 0; $c -lt $columns.Count; $c++) {
                if ($c -eq 1) { Get-SqlText $f[1] 15 } elseif ($f[$c] -match '^-?\d+$') { $f[$c] } else { throw "itemtype.dat: field $c of $($f[0]) is '$($f[$c])'" }
            }
            $rows.Add('(' + ($v -join ',') + ')')
        }
        if ($rows.Count + $skipped -ne $amount) { throw "itemtype.dat: Amount=$amount but $($rows.Count + $skipped) lines" }
    }
    'magictype' {
        $columns = 'type', 'sort', 'name', 'crime', 'ground', 'multi', 'target', 'level', 'use_mp', 'power',
                   'intone_speed', 'percent', 'step_secs', 'range', 'distance', 'status', 'need_prof', 'need_exp',
                   'need_level', 'use_xp', 'weapon_subtype', 'active_times', 'auto_active', 'floor_attr', 'auto_learn',
                   'learn_level', 'drop_weapon', 'use_ep', 'weapon_hit', 'use_item', 'next_magic', 'delay_ms',
                   'use_item_num'
        $b = Read-ClientFile 'MagicType.dat'
        $n = [BitConverter]::ToInt32($b, 0)
        $base = 4 + 4 * $n
        if ($n -le 0 -or $b.Length -ne $base + $n * 1584) { throw "MagicType.dat: $($b.Length) bytes do not hold $n records of 1584" }
        for ($k = 0; $k -lt $n; $k++) {
            $o = $base + $k * 1584
            $v = foreach ($c in 0..($columns.Count - 1)) {
                if ($c -lt 2) { [BitConverter]::ToInt32($b, $o + 4 * $c) }
                elseif ($c -eq 2) { $raw = $latin1.GetString($b, $o + 8, 16); Get-SqlText ($raw.Split([char]0)[0]) 15 }
                else { [BitConverter]::ToInt32($b, $o + 0x18 + 4 * ($c - 3)) }
            }
            $rows.Add('(' + ($v -join ',') + ')')
        }
    }
    'monstertype' {
        $columns = 'id', 'name', 'life', 'level'
        $text = $latin1.GetString([SfkClientDat]::Decrypt((Read-ClientFile 'Monster.dat')))
        $seen = @{}
        foreach ($m in [regex]::Matches($text, '(?m)^\[([^\]\r\n]+)\]\r?$')) {
            $start = $m.Index + $m.Length
            $next = $text.IndexOf("`n[", $start)
            $body = if ($next -lt 0) { $text.Substring($start) } else { $text.Substring($start, $next - $start) }
            $h = @{}
            foreach ($kv in ($body -split "`r?`n")) { if ($kv -match '^\s*([^=;]+?)\s*=\s*(.*?)\s*$') { $h[$Matches[1]] = $Matches[2] } }
            if ($h['TypeID'] -notmatch '^\d+$') { $skipped++; continue }
            if ($seen.ContainsKey($h['TypeID'])) { $skipped++; continue }
            $seen[$h['TypeID']] = $true
            $life = if ($h['MaxLife'] -match '^\d+$') { $h['MaxLife'] } else { '0' }
            $level = if ($h['Level'] -match '^\d+$') { $h['Level'] } else { '0' }
            $rows.Add('(' + (@($h['TypeID'], (Get-SqlText $m.Groups[1].Value 15), $life, $level) -join ',') + ')')
        }
        if (-not $rows.Count) { throw 'Monster.dat: no section with a TypeID (wrong key?)' }
    }
    'itemaddition' {
        $columns = 'id', 'typeid', 'level', 'life', 'attack_max', 'attack_min', 'defense', 'magic_atk', 'magic_def', 'dexterity', 'dodge'
        $id = 0
        foreach ($line in [IO.File]::ReadAllLines((Join-Path $Client 'ini\ItemAdd.ini'), $latin1)) {
            $t = $line.Trim()
            if (-not $t) { continue }
            $f = $t -split '\s+'
            if ($f.Count -ne 10 -or ($f | Where-Object { $_ -notmatch '^-?\d+$' })) { throw "ItemAdd.ini: not ten numbers: $t" }
            $id++
            $rows.Add('(' + ((@($id) + $f) -join ',') + ')')
        }
    }
    'point_allot' {
        $columns = 'id', 'profession', 'level', 'force', 'speed', 'health', 'soul'
        $b = Read-ClientFile 'AutoAllot.dat'
        $np = [BitConverter]::ToInt32($b, 0); $nl = [BitConverter]::ToInt32($b, 4)
        if ($b.Length -ne 4 * (2 + $np + $np * $nl * 4)) { throw "AutoAllot.dat: $($b.Length) bytes do not hold $np classes of $nl levels" }
        $id = 0
        for ($p = 0; $p -lt $np; $p++) {
            $prof = [BitConverter]::ToInt32($b, 8 + 4 * $p)
            if ($prof -notin 1, 2, 10) { $skipped++; continue }
            for ($l = 0; $l -lt $nl; $l++) {
                $o = 4 * (2 + $np + ($p * $nl + $l) * 4)
                $id++
                $v = @($id, $prof, ($l + 1)) + (0..3 | ForEach-Object { [BitConverter]::ToInt32($b, $o + 4 * $_) })
                $rows.Add('(' + ($v -join ',') + ')')
            }
        }
    }
    'goods' {
        $columns = 'id', 'ownerid', 'itemtype', 'moneytype'
        $id = 0; $owner = $null; $money = '0'
        foreach ($line in [IO.File]::ReadAllLines((Join-Path $Client 'ini\Shop.dat'), $latin1)) {
            if ($line -match '^\s*\[Shop') { $owner = $null; $money = '0' }
            elseif ($line -match '^\s*ID\s*=\s*(\d+)') { $owner = $Matches[1] }
            elseif ($line -match '^\s*MoneyType\s*=\s*(\d+)') { $money = $Matches[1] }
            elseif ($line -match '^\s*Item\d+\s*=\s*(\d+)') {
                if ($null -eq $owner) { throw "Shop.dat: an item before its shop's ID: $line" }
                $id++
                $rows.Add("($id,$owner,$($Matches[1]),$money)")
            }
        }
    }
}

$sb = New-Object Text.StringBuilder
$head = "INSERT INTO cq_$Table (" + (($columns | ForEach-Object { '`' + $_ + '`' }) -join ',') + ") VALUES`n"
for ($i = 0; $i -lt $rows.Count; $i += 500) {
    [void]$sb.Append($head)
    [void]$sb.Append(($rows.GetRange($i, [Math]::Min(500, $rows.Count - $i)) -join ",`n"))
    [void]$sb.Append(";`n")
}
[IO.File]::WriteAllBytes([IO.Path]::GetFullPath($Out), $latin1.GetBytes($sb.ToString()))
Write-Output ("cq_{0}: {1} rows written to {2}; {3} skipped (repeated or without an id)" -f $Table, $rows.Count, $Out, $skipped)
