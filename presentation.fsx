#!/usr/bin/fsi
open System
open System.IO
let (^) f x = f x
type Line =
  { Algorithm: string
    Processes: int
    Amount: int
    Time: int }
let source = 
    File.ReadAllLines("report.txt")
    |> Seq.map ^ fun line -> 
        let items = line.Split(":")
        { Algorithm = items.[0]
          Processes = Convert.ToInt32 items.[1] 
          Amount = Convert.ToInt32 items.[2]
          Time = Convert.ToInt32 items.[3]}
printfn "%A" source