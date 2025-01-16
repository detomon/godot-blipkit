#!/usr/bin/env python3
from glob import glob
from pathlib import Path
from types import SimpleNamespace
import argparse
import os
import re
import xml.etree.ElementTree as ET

methods = {}
members = {}
constants = {}
classes = {}


def attrib(element: ET.Element, attr_name: str) -> str:
    if attr_name not in element.attrib:
        return ""
    return element.attrib[attr_name]


def name_to_anchor(name: str) -> str:
    name = re.sub(r"[^\w_ ]", "", name)
    name = name.replace(" ", "-")
    name = name.lower()

    return name


def bbcode_replace_member(match: re.Match) -> str:
    global members
    name = match.group(1)

    if name not in methods:
        # TODO: Link to other object.
        return f"`{name}`"

    anchor = name_to_anchor(members[name].name_full)

    return f"[`{name}`](#{anchor})"


def bbcode_replace_method(match: re.Match) -> str:
    global methods
    name = match.group(1)

    if name not in methods:
        # TODO: Link to other object.
        return f"`{name}()`"

    anchor = name_to_anchor(methods[name].name_full)

    return f"[`{name}()`](#{anchor})"


def bbcode_replace_constant(match: re.Match) -> str:
    global constants
    name = match.group(1)

    if name not in constants:
        # TODO: Link to other object.
        return f"`{name}`"

    constant = constants[name]
    anchor = name_to_anchor(f"{name}")

    return f"[`{name}`](#{anchor})"


def bbcode_replace_enum(match: re.Match) -> str:
    name = match.group(1)
    anchor = name_to_anchor(f"enum {name}")
    return f"[`{name}`](#{anchor})"


def bbcode_replace_class(match: re.Match) -> str:
    global classes
    name = match.group(1)
    link = None

    if name in classes:
        link = classes[name]
    else:
        class_name = name.lower()
        link = f"https://docs.godotengine.org/en/stable/classes/class_{class_name}.html"

    return f"[`{name}`]({link})"


def bbcode_to_markdown(text: str) -> str:
    text = text.strip()
    lines = text.splitlines()
    new_lines = []
    code = ""
    code_lang = ""
    state = ""

    for line in lines:
        # Remove whitespaces but keep spaces at beginning for code blocks.
        line = line.lstrip("\t").rstrip()

        if state == "code":
            if re.search(r"\[gdscript\]", line):
                code_lang = "gdscript"

            elif re.search(r"\[/gdscript\]", line):
                pass

            # 'codeblock' or 'codeblocks'
            elif re.search(r"\[/codeblocks?\]", line):
                new_lines.append(f"```{code_lang}\n{code}```")
                state = ""

            else:
                # # Replace indention with tab.
                # indentation = re.search(r"^\s+", line, flags=re.MULTILINE)
                # if indentation:
                #   line = line.replace("    ", "\t")
                code += f"{line}\n"

        # Paragraph.
        else:
            if re.search(r"\[codeblocks\]", line):
                state = "code"
                code = ""
                code_lang = "gdscript" # Default.
                continue

            elif re.search(r"\[codeblock\s+lang=(\w+)\]", line):
                match = re.search(r"\[codeblock\s+lang=(\w+)\]", line)
                state = "code"
                code = ""
                code_lang = match.group(1)
                continue

            # Remove indentation.
            line = re.sub(r"^\t+", r"", line, flags=re.MULTILINE)
            # Italic.
            line = re.sub(r"\[i\](.*?)\[/i\]", r"*\1*", line)
            # Bold.
            line = re.sub(r"\[b\](.*?)\[/b\]", r"**\1**", line)
            # Inline code.
            line = re.sub(r"\[code\](.*?)\[/code\]", r"`\1`", line)
            # Parameter.
            line = re.sub(r"\[param (.*?)]", r"`\1`", line)
            # Class name.
            line = re.sub(r"\[([A-Z]\w+)\]", bbcode_replace_class, line)
            # Type name.
            line = re.sub(r"\[(bool|float|int)\]", r"`\1`", line)
            # Member link.
            line = re.sub(r"\[member (.*?)]", bbcode_replace_member, line)
            # Method link.
            line = re.sub(r"\[method (.*?)]", bbcode_replace_method, line)
            # Constant link.
            line = re.sub(r"\[constant (.*?)]", bbcode_replace_constant, line)
            # Enum link.
            line = re.sub(r"\[enum (.*?)]", bbcode_replace_enum, line)

            new_lines.append(f"{line}\n")

    return "\n".join(new_lines)


def prepare_tutorial(element: ET.Element) -> SimpleNamespace:
    title = attrib(element, "title")
    url = element.text.strip()
    name_link = f"[{title}]({url})"

    return SimpleNamespace(**{
        "title": title,
        "url": url,
        "name_link": name_link,
    })


def prepare_members(elements: [ET.Element]) -> dict:
    members = {}

    for element in elements:
        name = attrib(element, "name")
        type = attrib(element, "type")
        setter = attrib(element, "setter")
        getter = attrib(element, "getter")
        default = attrib(element, "default")
        description = element.text

        name_full = f"`{type} {name}`"
        anchor = name_to_anchor(name_full)
        name_link = f"*{type}* [**`{name}`**](#{anchor})"

        members[name] = SimpleNamespace(**{
            "name": name,
            "type": type,
            "name_full": name_full,
            "name_link": name_link,
            "anchor": anchor,
            "setter": setter,
            "getter": getter,
            "default": default,
            "description": description,
        })

    return members


def prepare_method_param(element: ET.Element) -> SimpleNamespace:
    name = attrib(element, "name")
    type = attrib(element, "type")
    default = attrib(element, "default")

    name_full = f"{name}: {type}"
    if default:
        name_full += f" = {default}"

    return SimpleNamespace(**{
        "name": name,
        "name_full": name_full,
        "type": type,
        "default": default,
    })


def prepare_methods(elements: [ET.Element]) -> dict:
    methods = {}

    for element in elements:
        name = attrib(element, "name")
        qualifiers = attrib(element, "qualifiers")
        return_type = attrib(element.find("./return"), "type")
        params = element.findall("./param")
        description = element.find("./description").text

        if qualifiers:
            qualifiers = f" {qualifiers}"
        params = map(prepare_method_param, params)

        args = ", ".join(map(lambda x: x.name_full, params))
        name_full = f"`{return_type} {name}({args}){qualifiers}`"
        anchor = name_to_anchor(name_full)
        name_link = f"*{return_type}* [**`{name}`**](#{anchor})({args}){qualifiers}"

        methods[name] = SimpleNamespace(**{
            "name": name,
            "name_full": name_full,
            "name_link": name_link,
            "anchor": anchor,
            "qualifiers": qualifiers,
            "return_type": return_type,
            "params": params,
            "description": description,
        })

    return methods


def prepare_constants(elements: [ET.Element]) -> dict:
    constants = {}

    for element in elements:
        name = attrib(element, "name")
        value = attrib(element, "value")
        enum = attrib(element, "enum")
        description = element.text

        constants[name] = SimpleNamespace(**{
            "name": name,
            "value": value,
            "enum": enum,
            "description": description,
        })

    return constants


def prepare_constant_groups(constants: dict) -> dict:
    groups = {}

    for constant in constants.values():
        name = constant.name
        enum = constant.enum

        if enum not in groups:
            groups[enum] = SimpleNamespace(**{
                "name": enum,
                "values": {},
            })

        groups[enum].values[name] = constant

    return groups.values()


def make_markdown(root: ET.Element) -> str:
    global methods, members, constants

    class_name = root.attrib["name"]
    inherits = root.attrib["inherits"]
    brief_description = root.find("./brief_description")
    description = root.find("./description")
    tutorials = root.findall("./tutorials/link")
    methods = root.findall("./methods/method")
    members = root.findall("./members/member")
    constants = root.findall("./constants/constant")

    tutorials = list(map(prepare_tutorial, tutorials))
    methods = prepare_methods(methods)
    members = prepare_members(members)
    constants = prepare_constants(constants)
    constant_groups = prepare_constant_groups(constants)
    brief_description = bbcode_to_markdown(brief_description.text).strip()
    description = bbcode_to_markdown(description.text)

    output = ""
    output += f"# Class: {class_name}\n\
\n\
Inherits: *{inherits}*\n\
\n\
**{brief_description}**\n\
\n\
## Description\n\
\n\
{description}\n"

    if tutorials:
        output += f"## Online Tutorials\n\n"
        for item in tutorials:
            output += f"- {item.name_link}\n"
        output += "\n"

    if members:
        output += f"## Properties\n\n"
        for item in members.values():
            output += f"- {item.name_link}\n"
        output += "\n"

    if methods:
        output += f"## Methods\n\n"
        for item in methods.values():
            output += f"- {item.name_link}\n"
        output += "\n"

    if constant_groups:
        enum_groups = list(filter(lambda enum : enum.name != "", constant_groups))
        const_groups = list(filter(lambda enum : enum.name == "", constant_groups))

        if len(enum_groups):
            output += f"## Enumerations\n\n"
            for enum in enum_groups:
                output += f"### enum `{enum.name}`\n\n"
                for item in enum.values.values():
                    description = bbcode_to_markdown(item.description).strip()
                    output += f"- `{item.name}` = `{item.value}`\n"
                    output += f"\t- {description}\n"
                output += "\n"

        if len(const_groups):
            output += f"## Constants\n\n"
            for enum in const_groups:
                for item in enum.values.values():
                    description = bbcode_to_markdown(item.description).strip()
                    output += f"- `{item.name}` = `{item.value}`\n"
                    output += f"\t- {description}\n"
                output += "\n"

    if members:
        output += f"## Property Descriptions\n\n"
        for item in members.values():
            description = bbcode_to_markdown(item.description)
            output += f"### {item.name_full}\n\n"
            if item.default:
                output += f"*Default*: `{item.default}`\n\n"
            output += f"{description}\n"
        output += "\n"

    if methods:
        output += f"## Method Descriptions\n\n"
        for item in methods.values():
            description = bbcode_to_markdown(item.description)
            output += f"### {item.name_full}\n\n"
            output += f"{description}\n"
        output += "\n"

    return output


def markdown(file: str) -> str:
    tree = ET.parse(file)
    root = tree.getroot()

    return make_markdown(root)


def make_markdown_docs(target_dir: str, source: list) -> None:
    global classes

    # Remove all files.
    files = glob(f"{target_dir}/*.md")
    for file_name in files:
        os.unlink(file_name)

    # List classes.
    for file_name in source:
        class_name = Path(file_name).stem
        target_name = f"{class_name}.md"
        classes[class_name] = target_name

    # Create files.
    for source_file in source:
        target_file = f"{target_dir}/{target_name}"
        class_name = Path(source_file).stem
        target_file = f"{target_dir}/{class_name}.md"

        output = markdown(source_file)
        with open(target_file, "w", encoding="utf-8") as f:
            f.write(output)

    class_names = list(classes.keys())
    class_names.sort()

    # Write overview.
    readme_file = f"{target_dir}/README.md"
    with open(readme_file, "w", encoding="utf-8") as f:
        f.write("# Classes\n\n")
        for class_name in class_names:
            f.write(f"- [`{class_name}`]({class_name}.md)\n")


if __name__ == "__main__":
    parser = argparse.ArgumentParser("markdown")
    parser.add_argument("sources", help="Input XML files.", type=str)
    parser.add_argument("target", help="Output directory.", type=str)
    args = parser.parse_args()

    sources = glob(args.sources)
    make_markdown_docs(args.target, sources)
