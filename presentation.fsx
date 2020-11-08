#!/usr/bin/fsi
open System
open System.IO
let (^) f x = f x

let multiThread, singleThread = "parallel", "single"

type Line =
  { Algorithm: string
    Processes: int
    Amount: int
    Time: float }
let source = 
    File.ReadAllLines("report.txt")
    |> Seq.map ^ fun line -> 
        let items = line.Split(":")
        { Algorithm = items.[0]
          Processes = if items.[0] = multiThread then Convert.ToInt32 items.[1] else 1
          Amount = Convert.ToInt32 items.[2]
          Time = float ^ Convert.ToInt32 items.[3]}

let accelerationBy algorithm =
    source
    |> Seq.filter ^ fun {Algorithm = a} -> a = algorithm
    |> Seq.groupBy ^ fun x -> x.Amount
    |> Seq.sortBy ^ fun (k, _) -> k
    |> Seq.map ^ fun (amount, items) -> amount, Seq.averageBy (fun x -> x.Time) items
    |> Seq.windowed 2
    |> Seq.map ^ fun [|aa, a; ab, b|] -> sprintf "%i - %i" aa ab, a - b
    |> List.ofSeq

let formatAcceleration data =
    for (where, howMuch) in data do
        printfn "%s:%.2f" where howMuch
// Приращение времени выполнения от количества данных

// formatAcceleration ^ accelerationBy multiThread
// formatAcceleration ^ accelerationBy singleThread

type 'a TableRowValue =
    | Header of string
    | Value of 'a

let accelerationByThreadCount =
    source
    |> Seq.filter ^ fun {Algorithm = a} -> a = multiThread
    |> Seq.groupBy ^ fun x -> x.Processes, x.Amount
    |> Seq.sortBy ^ fun (k, _) -> k
    |> Seq.map ^ fun ((p, a), xs) -> p, a, (Seq.exactlyOne xs).Time
    |> List.ofSeq

let formatShit data = 
    for (p, a, t) in data do
        (sprintf "%i:%i:%.2f" p a t).Replace('.', ',')
        |> printfn "%s"

//formatShit accelerationByThreadCount
        
let parallelAdvantage =
    List.ofSeq source
    // |> List.partition ^ fun x -> x.Algorithm = multiThread
    |> List.groupBy ^ fun x -> x.Amount, x.Algorithm
    |> List.map ^ fun ((a, alg), xs) -> a, alg, xs |> List.averageBy ^ fun x -> x.Time
    |> List.groupBy ^ fun (a, alg, t) -> a
    |> List.map ^ fun (a, xs) -> a, 
                                 xs |> List.where (fun (a, alg, t) -> alg = multiThread) |> List.map (fun (a, alg, t) -> t) |> List.exactlyOne,
                                 xs |> List.where (fun (a, alg, t) -> alg = singleThread) |> List.map (fun (a, alg, t) -> t) |> List.exactlyOne

let formatAdvantage data =
    for (amount, pt, lt) in data do
        (sprintf "%i:%f:%f" amount pt lt).Replace('.', ',')
        |> printfn "%s"

formatAdvantage parallelAdvantage