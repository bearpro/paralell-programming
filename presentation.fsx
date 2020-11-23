#!/usr/bin/fsi
open System
open System.IO

#nowarn "46"
let (^) f x = f x

let multiThread, singleThread = "parallel", "single"

let path = fsi.CommandLineArgs.[1]

let formatFloat (value: float) =
    (sprintf "%.2f" value)
        .Replace('.', ',')

let formatFloatA (tw:TextWriter) value =
    tw.Write (formatFloat value)
    

type Line =
  { Algorithm: string
    Processes: int
    Amount: int
    Time: float }

let allResults = 
    let parseLine (line: string) =
        let items = line.Split(":")
        { Algorithm = items.[0]
          Processes = if items.[0] = multiThread then Convert.ToInt32 items.[1] else 1
          Amount = Convert.ToInt32 items.[2]
          Time = float ^ Convert.ToInt32 items.[3]}
    
    File.ReadAllLines(path)
    |> Seq.map parseLine

let parallelResults = allResults |> Seq.where ^ fun line -> line.Algorithm = multiThread

let linearResults = allResults |> Seq.where ^ fun line -> line.Algorithm = singleThread

module Metrics = 
    let efficiency acceleration (processes: int) : float = 
        acceleration / (float processes)
    let acceleration parallelTime linearTime : float = 
        linearTime / parallelTime
    let cost (processes: int) parallelTime = 
        (float processes) * parallelTime


type MetricsLine = 
  { Acceleration: float
    Amount: int
    Efficiency: float
    Processes: int 
    Cost: float }

let metrics (source: seq<Line>) =
    source
    |> Seq.map ^ fun parallel -> 
        let linear = linearResults |> Seq.find ^ fun x -> x.Amount = parallel.Amount
        let acceleration = Metrics.acceleration parallel.Time linear.Time
        let efficiency = Metrics.efficiency acceleration parallel.Processes
        let cost = Metrics.cost parallel.Processes parallel.Time
        { Processes = parallel.Processes
          Acceleration = acceleration
          Amount = linear.Amount
          Efficiency = efficiency
          Cost = cost }
let matrix selector source = 
    // Потоков:Ускорение_при_N1:Ускорение_при_N2...
    source
    |> Seq.sortBy ^ fun x -> x.Amount 
    |> Seq.groupBy ^ fun x -> x.Amount 
    |> Seq.map ^ fun (x, xs) -> 
        let values = 
            xs
            |> Seq.sortBy ^ fun x -> x.Amount 
            |> Seq.fold (fun a b -> sprintf "%s:%s" a (formatFloat (selector b))) ""
        sprintf "%d%s" x values

let format label data =
    printfn "%s" label
    for item in data do
        printfn "%s" item

allResults
|> metrics
|> matrix ^ fun x -> x.Acceleration
|> format "Ускорение"

allResults
|> metrics
|> matrix ^ fun x -> x.Efficiency
|> format "Эффективность"

parallelResults
|> metrics
|> matrix ^ fun x -> x.Cost
|> format "Стоимость"
