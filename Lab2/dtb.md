# DeviceTree Markdown  

## Base Device Node Types
All devicetrees shall have a root node and the following nodes shall be present at the root of all devicetrees:

- One /cpus node 
- At least one /memory node

## Versioning
The document describes only version 17 of the format. DTSpec complimant boot programs shall provide a devicetree of version 17 or later, and should provide a devicetree of a version that is a backwards compatible with version 16.

## Header
The layout of the header for the devicetree is defined by the following C structure.

- magic <br>
	This field shall contain the value **0xd00dfeed(big-endian)**

- totalsize<br>
	This field shall contain the totalsize in bytes of the devicetree data structure. <br>
	This shall encompass all section of the structure: <br>
	the header, the memory reservation block, structure block and strings block. as well as any free space gaps b/w the blocks or after the final block.

- off_dt_struct <br>
	This field shall contain the offset in bytes of the structure block from the beginning of the header.

- off_dt_strings<br>
	This field shall contain the offset in bytes of the strings block from the beginning of the header

- off_mem_rsvmap<br>
	This field shall contain the offset in bytes of the memory reservation block from the beginning of the header.

- version<br>
	<p>This field shall contain the version of the devicetree data structure.</p> An DTSpec boot program may provide the devicetree of a later version, in which case this field shall contain the version number defined in whichever later document gives the details of that version.

## Format
The memory reservation block consists of a list of pairs of 64-bit big-endian integers, each pair being represented by the following C structure.
``` C=
struct fdt_reverse_entry {
	uint64_t address;
	uint64_t size;
}
```
Each pair gives the physical address and size in bytes of a reserved memory region. These given regions shall not overlap each other. The list of reserved blocks shall be terminated with an entry where both address and size are equal to 0.

## Structure Block 

### Lexical structure 
The structure block is compressed of a sequence of pieces, each beginning with a token, that is, a big-endian 32-bit integer.
<br>
The five token types are as follows:

