/**
*   Copyright (C) 2011  Xu Cheng, Yang Zhengyu ,Zuo Zhiheng
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
///:NTFS.h
#ifndef NTFS_H
#define NTFS_H

#include <Windows.h>
#include <WinIoCtl.h>

/* define NTFS on-disk structure */
/* Ref: http://www.installsetupconfig.com/win32programming/1996%20AppE_apnilife.pdf */

typedef struct
{
    ULONG Type;
    USHORT UsaOffset;
    USHORT UsaCount;
    USN Usn;
} NTFS_RECORD_HEADER, *PNTFS_RECORD_HEADER;
/**
* Members:
*   Type - The type of NTFS record.When the value of Type is considered as a sequence of four
*       one-byte characters, it normally spells an acronym for the type. Defined values include:
*       'FILE''INDX''BAAD''HOLE''CHKD'
*   UsaOffset - The offset, in bytes, from the start of the structure to the Update Sequence Array.
*   UsaCount - The number of values in the Update Sequence Array.
*   Usn - The Update Sequence Number of the NTFS record.
*/

typedef struct
{
    NTFS_RECORD_HEADER Ntfs;
    USHORT SequenceNumber;
    USHORT LinkCount;
    USHORT AttributesOffset;
    USHORT Flags; // 0x0001 = InUse, 0x0002 = Directory
    ULONG BytesInUse;
    ULONG BytesAllocated;
    ULONGLONG BaseFileRecord;
    USHORT NextAttributeNumber;
} FILE_RECORD_HEADER, *PFILE_RECORD_HEADER;
/**
* Members:
*   Ntfs - An NTFS_RECORD_HEADER structure with a Type of 'FILE'.
*   SequenceNumber - The number of times that the MFT entry has been reused.
*   LinkCount - The number of directory links to the MFT entry.
*   AttributeOffset - The offset, in bytes, from the start of the structure to the first attribute
*       of the MFT entry.
*   Flags - A bit array of flags specifying properties of the MFT entry.The values defined include:
*       InUse 0x0001     // The MFT entry is in use
*       Directory 0x0002 // The MFT entry represents a directory
*   BytesInUse - The number of bytes used by the MFT entry.
*   BytesAllocated - The number of bytes allocated for the MFT entry.
*   BaseFileRecord - If the MFT entry contains attributes that overflowed a base MFT entry, this member
*       contains the file reference number of the base entry; otherwise, it contains zero.
*   NextAttributeNumber - The number that will be assigned to the next attribute added to the MFT entry.
* Remarks:
*   An entry in the MFT consists of a FILE_RECORD_HEADER followed by a sequence of
*   attributes.
*/

typedef enum
{
    AttributeStandardInformation = 0x10,
    AttributeAttributeList = 0x20,
    AttributeFileName = 0x30,
    AttributeObjectId = 0x40,
    AttributeSecurityDescriptor = 0x50,
    AttributeVolumeName = 0x60,
    AttributeVolumeInformation = 0x70,
    AttributeData = 0x80,
    AttributeIndexRoot = 0x90,
    AttributeIndexAllocation = 0xA0,
    AttributeBitmap = 0xB0,
    AttributeReparsePoint = 0xC0,
    AttributeEAInformation = 0xD0,
    AttributeEA = 0xE0,
    AttributePropertySet = 0xF0,
    AttributeLoggedUtilityStream = 0x100,
    AttributeEnd = 0xFFFFFFFF
} ATTRIBUTE_TYPE, *PATTRIBUTE_TYPE;

typedef struct
{
    ATTRIBUTE_TYPE AttributeType;
    ULONG Length;
    BOOLEAN Nonresident;
    UCHAR NameLength;
    USHORT NameOffset;
    USHORT Flags; // 0x0001 = Compressed 0x4000 = Encrypted 0x8000 = Sparse file
    USHORT AttributeNumber;
} ATTRIBUTE, *PATTRIBUTE;
/**
* Members:
*   AttributeType - The type of the attribute.The following types are defined above.
*   Length - The size, in bytes, of the resident part of the attribute.
*   Nonresident - Specifies, when true, that the attribute value is nonresident.
*   NameLength - The size, in characters, of the name (if any) of the attribute.
*   NameOffset - The offset, in bytes, from the start of the structure to the attribute name.The attribute
*       name is stored as a Unicode string.
*   Flags - A bit array of flags specifying properties of the attribute.The values defined include:
*       Compressed 0x0001  // The attribute is compressed
*       Encrypted 0x4000   // The attribute is encrypted
*       Sparse file 0x8000 // The attribute is sparse file
*   AttributeNumber - A numeric identifier for the instance of the attribute.
*/

typedef struct
{
    ATTRIBUTE Attribute;
    ULONG ValueLength;
    USHORT ValueOffset;
    USHORT Flags; // 0x0001 = Indexed
} RESIDENT_ATTRIBUTE, *PRESIDENT_ATTRIBUTE;
/**
* Members:
*   Attribute - An ATTRIBUTE structure containing members common to resident and nonresident
*       attributes.
*   ValueLength - The size, in bytes, of the attribute value.
*   ValueOffset - The offset, in bytes, from the start of the structure to the attribute value.
*   Flags - A bit array of flags specifying properties of the attribute.The values defined include:
*       Indexed 0x0001 // The attribute is indexed
*/

typedef struct
{
    ATTRIBUTE Attribute;
    ULONGLONG LowVcn;
    ULONGLONG HighVcn;
    USHORT RunArrayOffset;
    UCHAR CompressionUnit;
    UCHAR AlignmentOrReserved[5];
    ULONGLONG AllocatedSize;
    ULONGLONG DataSize;
    ULONGLONG InitializedSize;
    ULONGLONG CompressedSize; // Only when compressed
} NONRESIDENT_ATTRIBUTE, *PNONRESIDENT_ATTRIBUTE;
/**
* Members:
*   Attribute - An ATTRIBUTE structure containing members common to resident and nonresident
*       attributes.
*   LowVcn - The lowest valid Virtual Cluster Number (VCN) of this portion of the attribute value.
*       Unless the attribute value is very fragmented (to the extent that an attribute list is
*       needed to describe it), there is only one portion of the attribute value, and the value of
*       LowVcn is zero.
*   HighVcn - The highest valid VCN of this portion of the attribute value.
*   RunArrayOffset - The offset, in bytes, from the start of the structure to the run array that contains the
*       mappings between VCNs and Logical Cluster Numbers (LCNs).
*   CompressionUnit - The compression unit for the attribute expressed as the logarithm to the base two of
*       the number of clusters in a compression unit. If CompressionUnit is zero, the attribute
*       is not compressed.
*   AllocatedSize - The size, in bytes, of disk space allocated to hold the attribute value.
*   DataSize - The size, in bytes, of the attribute value.This may be larger than the AllocatedSize if
*       the attribute value is compressed or sparse.
*   InitializedSize - The size, in bytes, of the initialized portion of the attribute value.
*   CompressedSize - The size, in bytes, of the attribute value after compression.This member is only present
*       when the attribute is compressed.
*/

typedef struct
{
    ULONGLONG CreationTime;
    ULONGLONG ChangeTime;
    ULONGLONG LastWriteTime;
    ULONGLONG LastAccessTime;
    ULONG FileAttributes;
    ULONG AlignmentOrReservedOrUnknown[3];
    ULONG QuotaId; // NTFS 3.0 only
    ULONG SecurityId; // NTFS 3.0 only
    ULONGLONG QuotaCharge; // NTFS 3.0 only
    USN Usn; // NTFS 3.0 only
} STANDARD_INFORMATION, *PSTANDARD_INFORMATION;
/**
* Members:
*   CreationTime - The time when the file was created in the standard time format (that is, the number of
*       100-nanosecond intervals since January 1, 1601).
*   ChangeTime - The time when the file attributes were last changed in the standard time format (that
*       is, the number of 100-nanosecond intervals since January 1, 1601).
*   LastWriteTime - The time when the file was last written in the standard time format (that is, the number
*       of 100-nanosecond intervals since January 1, 1601).
*   LastAccessTime - The time when the file was last accessed in the standard time format (that is, the number
*       of 100-nanosecond intervals since January 1, 1601).
*   FileAttributes - The attributes of the file. Defined attributes include:
*       FILE_ATTRIBUTE_READONLY,FILE_ATTRIBUTE_HIDDEN,FILE_ATTRIBUTE_SYSTEM,FILE_ATTRIBUTE_DIRECTORY,FILE_ATTRIBUTE_ARCHIVE,
*       FILE_ATTRIBUTE_NORMAL,FILE_ATTRIBUTE_TEMPORARY,FILE_ATTRIBUTE_SPARSE_FILE,FILE_ATTRIBUTE_REPARSE_POINT,
*       FILE_ATTRIBUTE_COMPRESSED,FILE_ATTRIBUTE_OFFLINE,FILE_ATTRIBUTE_NOT_CONTENT_INDEXED,FILE_ATTRIBUTE_ENCRYPTED
*   AlignmentOrReservedOrUnknown - Normally contains zero. Interpretation unknown.
*   QuotaId - A numeric identifier of the disk quota that has been charged for the file (probably an
*       index into the file ¡°\$Extend\$Quota¡±). If quotas are disabled, the value of QuotaId is
*       zero.This member is only present in NTFS 3.0. If a volume has been upgraded from
*       an earlier version of NTFS to version 3.0, this member is only present if the file has
*       been accessed since the upgrade.
*   SecurityId - A numeric identifier of the security descriptor that applies to the file (probably an
*       index into the file ¡°\$Secure¡±).This member is only present in NTFS 3.0. If a volume
*       has been upgraded from an earlier version of NTFS to version 3.0, this member is
*       only present if the file has been accessed since the upgrade.
*   QuotaCharge - The size, in bytes, of the charge to the quota for the file. If quotas are disabled, the
*       value of QuotaCharge is zero.This member is only present in NTFS 3.0. If a volume
*       has been upgraded from an earlier version of NTFS to version 3.0, this member is
*       only present if the file has been accessed since the upgrade.
*   Usn - The Update Sequence Number of the file. If journaling is not enabled, the value of
*       Usn is zero.This member is only present in NTFS 3.0. If a volume has been upgraded
*       from an earlier version of NTFS to version 3.0, this member is only present if the file
*       has been accessed since the upgrade.
* Remarks:
*   The standard information attribute is always resident.
*/

typedef struct
{
    ATTRIBUTE_TYPE AttributeType;
    USHORT Length;
    UCHAR NameLength;
    UCHAR NameOffset;
    ULONGLONG LowVcn;
    ULONGLONG FileReferenceNumber;
    USHORT AttributeNumber;
    USHORT AlignmentOrReserved[3];
} ATTRIBUTE_LIST, *PATTRIBUTE_LIST;
/**
* Members:
*   AttributeType - The type of the attribute.
*   Length - The size, in bytes, of the attribute list entry.
*   NameLength - The size, in characters, of the name (if any) of the attribute.
*   NameOffset - The offset, in bytes, from the start of the ATTRIBUTE_LIST structure to the attribute
*       name.The attribute name is stored as a Unicode string.
*   LowVcn - The lowest valid Virtual Cluster Number (VCN) of this portion of the attribute value.
*   FileReferenceNumber - The file reference number of the MFT entry containing the NONRESIDENT_ATTRIBUTE
*       structure for this portion of the attribute value.
*   AttributeNumber - A numeric identifier for the instance of the attribute.
* Remarks:
*   The attribute list attribute is always nonresident and consists of an array of
*   ATTRIBUTE_LIST structures.
*   An attribute list attribute is only needed when the attributes of a file do not fit in a
*   single MFT record. Possible reasons for overflowing a single MFT entry include:
*   * The file has a large numbers of alternate names (hard links)
*   * The attribute value is large, and the volume is badly fragmented
*   * The file has a complex security descriptor (does not affect NTFS 3.0)
*   * The file has many streams
*/

typedef struct
{
    ULONGLONG DirectoryFileReferenceNumber;
    ULONGLONG CreationTime; // Saved when filename last changed
    ULONGLONG ChangeTime; // ditto
    ULONGLONG LastWriteTime; // ditto
    ULONGLONG LastAccessTime; // ditto
    ULONGLONG AllocatedSize; // ditto
    ULONGLONG DataSize; // ditto
    ULONG FileAttributes; // ditto
    ULONG AlignmentOrReserved;
    UCHAR NameLength;
    UCHAR NameType; // 0x01 = Long, 0x02 = Short
    WCHAR Name[1];
} FILENAME_ATTRIBUTE, *PFILENAME_ATTRIBUTE;
/**
* Members:
*   DirectoryFileReferenceNumber - The file reference number of the directory in which the filename is entered.
*   CreationTime - The time when the file was created in the standard time format (that is. the number of
*       100-nanosecond intervals since January 1, 1601).This member is only updated when the filename changes
*       and may differ from the field of the same name in the STANDARD_INFORMATION structure.
*   ChangeTime - The time when the file attributes were last changed in the standard time format (that is, the
*       number of 100-nanosecond intervals since January 1, 1601).This member is only updated when the filename
*       changes and may differ from the field of the same name in the STANDARD_INFORMATION structure.
*   LastWriteTime - The time when the file was last written in the standard time format (that is, the number of
*       100-nanosecond intervals since January 1, 1601).This member is only updated when the filename changes
*       and may differ from the field of the same name in the STANDARD_INFORMATION structure.
*   LastAccessTime - The time when the file was last accessed in the standard time format (that is, the number
*       of 100-nanosecond intervals since January 1, 1601).This member is only updated when the filename changes
*       and may differ from the field of the same name in the STANDARD_INFORMATION structure.
*   AllocatedSize - The size, in bytes, of disk space allocated to hold the attribute value.This member is
*       only updated when the filename changes.
*   DataSize - The size, in bytes, of the attribute value.This member is only updated when the filename changes.
*   FileAttributes - The attributes of the file.This member is only updated when the filename changes and
*       may differ from the field of the same name in the STANDARD_INFORMATION structure.
*   NameLength - The size, in characters, of the filename.
*   NameType - The type of the name.A type of zero indicates an ordinary name, a type of one indicates a long name
*       corresponding to a short name, and a type of two indicates a short name corresponding to a long name.
*   Name - The name, in Unicode, of the file.
* Remarks:
*   The filename attribute is always resident.
*/

#pragma pack(push, 1)
typedef struct
{
    UCHAR Jump[3];
    UCHAR Format[8];
    USHORT BytesPerSector;
    UCHAR SectorsPerCluster;
    USHORT BootSectors;
    UCHAR Mbz1;
    USHORT Mbz2;
    USHORT Reserved1;
    UCHAR MediaType;
    USHORT Mbz3;
    USHORT SectorsPerTrack;
    USHORT NumberOfHeads;
    ULONG PartitionOffset;
    ULONG Reserved2[2];
    ULONGLONG TotalSectors;
    ULONGLONG MftStartLcn;
    ULONGLONG Mft2StartLcn;
    ULONG ClustersPerFileRecord;
    ULONG ClustersPerIndexBlock;
    ULONGLONG VolumeSerialNumber;
    UCHAR Code[0x1AE];
    USHORT BootSignature;
} BOOT_BLOCK, *PBOOT_BLOCK;
#pragma pack(pop)
/* ***************************** */

bool File_Info_by_NTFS (wchar_t Path, DWORDLONG frn
                        , FILETIME *pOutTimeCreate
                        , FILETIME *pOutTimeLastWrite
                        , DWORDLONG *pOutSize);

/* some class and function needed for File_Info_by_NTFS */
#include <hash_map>
namespace NTFS
{
    typedef struct
    {
        __int16 BytesPerFileRecordSegment;
        __int16 BytesPerCluster;
    } VolNTFSInfoNode;

    class VolNTFSInfoCache
    {
    public:
        VolNTFSInfoCache() {}
        ~VolNTFSInfoCache() {}
        VolNTFSInfoNode getInfoNode (wchar_t Path);
    private:
        std::hash_map<wchar_t, VolNTFSInfoNode> m_data;
    };

    extern VolNTFSInfoCache *data_volNTFSInfoCache;
};
/* *********************************************************** */

#endif
///:~