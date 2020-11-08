#!/usr/bin/fsi
open System
open System.IO

#nowarn "46"
let (^) f x = f x

let multiThread, singleThread = "parallel", "single"

let path = fsi.CommandLineArgs.[1]

let formatFloat (tw:TextWriter) value =
    (sprintf "%.2f" value)
        .Replace('.', ',')
    |> tw.Write 

type Line =
  { Algorithm: string
    Processes: int
    Amount: int
    Time: float }

let source = 
    let parseLine (line: string) =
        let items = line.Split(":")
        { Algorithm = items.[0]
          Processes = if items.[0] = multiThread then Convert.ToInt32 items.[1] else 1
          Amount = Convert.ToInt32 items.[2]
          Time = float ^ Convert.ToInt32 items.[3]}
    
    File.ReadAllLines(path)
    |> Seq.map parseLine
        

let parallelResults = source |> Seq.where ^ fun line -> line.Algorithm = multiThread
let linearResults = source |> Seq.where ^ fun line -> line.Algorithm = singleThread

module Metrics = 
    let efficiency acceleration (processes: int) : float = 
        acceleration / (float processes)
    let acceleration parallelTime linearTime : float = 
        linearTime / parallelTime
    let cost (processes: int) parallelTime = 
        (float processes) * parallelTime

let accelerationByThreadCount =
    parallelResults
    |> Seq.map (fun parallel -> 
        let linear = linearResults |> Seq.find ^ fun x -> x.Amount = parallel.Amount
        let acceleration = Metrics.acceleration parallel.Time linear.Time
        let efficiency = Metrics.efficiency acceleration parallel.Processes
        {| Processes = parallel.Processes
           Acceleration = acceleration
           Amount = linear.Amount
           Efficiency = efficiency |})

let formatAcceleration 
    (data: seq<{| Acceleration: float; Amount: int; Efficiency: float; Processes: int |}>) =
    printfn "Ускорение и эффективность"
    for item in data do
        printfn "%a:%d:%d:%a" formatFloat item.Acceleration item.Processes item.Amount formatFloat item.Efficiency


let costByProcessesAndAmount =
    parallelResults
    |> Seq.map (fun x -> 
        {| Cost = Metrics.cost x.Processes x.Time
           Processes = x.Processes
           Amount = x.Amount |})

let formatCost 
    (data: seq<{| Amount: int; Cost: float; Processes: int |}>) =
    printfn "Стоимость"
    for item in data do
        printfn "%d:%d:%a" item.Amount item.Processes formatFloat item.Cost 

accelerationByThreadCount |> formatAcceleration
costByProcessesAndAmount  |> formatCost

