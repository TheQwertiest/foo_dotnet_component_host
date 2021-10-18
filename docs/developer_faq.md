---
title: Developer FAQ
nav_order: 4
---

# Developer FAQ
{: .no_toc }

## Table of contents
{: .no_toc .text-delta }

* TOC
{:toc}

---

Note: this section is WIP.

## .NET component host

As it's name suggests `.NET component host` is the core .NET component system and is responsible for loading your components and providing them with a core set of functionality. 

## IComponent

All .NET components have to implement the `IComponent` interface that gives them access to objects like foobar2000 playback controls or track information.

`IComponent` interface is provided via `dotnet_component_interface` NuGet package: <https://github.com/TheQwertiest/foo_dotnet_component_host/packages/>

## Examples

While there are no simple samples, you can start by checking the [`dotnet_title_bar` component](https://github.com/TheQwertiest/dotnet_title_bar).
