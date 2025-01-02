module PE2 where

import Text.Printf

type Point = (Int, Int)
type Dimensions = (Int, Int)
type Vector = (Int, Int)

getRounded :: Double -> Double
getRounded x = read s :: Double
               where s = printf "%.2f" x

castIntToDouble x = (fromIntegral x) :: Double

-------------------------------------------------------------------------------------------
----------------------- DO NOT CHANGE ABOVE OR FUNCTION SIGNATURES-------------------------
------------- DUMMY IMPLEMENTATIONS ARE GIVEN TO PROVIDE A COMPILABLE TEMPLATE ------------
------------------- REPLACE THEM WITH YOUR COMPILABLE IMPLEMENTATIONS ---------------------
-------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------------------------
getVector :: String -> Vector
getVector d = 

    if d== "NorthWest" then (-1,1)
    else if d=="NorthEast" then (1,1)
    else if d=="Stay" then (0,0)
    else if d=="SouthWest" then (-1,-1)
    else if d=="SouthEast" then (1,-1)
    else if d=="North" then (0,1)
    else if d=="South" then (0,-1)
    else if d=="West" then (-1,0)
    else  (1,0)


-------------------------------------------------------------------------------------------------------------------------------
getAllVectors :: [String] -> [Vector]
getAllVectors l = 

    map f l

    where
        f= getVector
    


-------------------------------------------------------------------------------------------------------------------------------
helperforPath ::  [String] -> [Point] -> [Point]
helperforPath actions target=
    if actions== [] then target
    else  helperforPath (tail actions) (target ++  [(x,y)])
        where
            x=fst (last target) + fst (getVector (head actions))
            y=snd (last target) + snd (getVector (head actions))
            
            
producePath :: Point -> [String] -> [Point]
producePath initial actions = 

    helperforPath actions [initial]
    


-------------------------------------------------------------------------------------------------------------------------------
helperforInArea :: [Point] -> Dimensions ->[Point] ->[Point]
helperforInArea path (bx,by) target=
    if path==[] then target
    else if x<0 ||x>=bx || y<0 || y>=by then target
    else helperforInArea (tail path) (bx,by) (target ++ [head path])
        where
        x=fst (head path)
        y=snd (head path)


takePathInArea :: [Point] -> Dimensions -> [Point]
takePathInArea path (bx, by) = 
    helperforInArea path (bx,by) []
-------------------------------------------------------------------------------------------------------------------------------


helperforRemaining :: [Point]-> [Point] -> [Point] -> [Point]
helperforRemaining newlist' objects target=
    if objects==[] then target
    else if not(head objects `elem` newlist') then helperforRemaining newlist' (tail objects) (target++ [head objects])
    else helperforRemaining  newlist' (tail objects) target

remainingObjects :: [Point] -> Dimensions -> [Point] -> [Point]
remainingObjects path border objects = 
    helperforRemaining newlist' objects []
        where
            newlist'=helperforInArea path border []







-------------------------------------------------------------------------------------------------------------------------------
helperforSuccess :: [[Point]] -> Dimensions -> [Point] ->  Int -> Int ->Double
helperforSuccess paths border objects  target1 target2=

    if paths== [] then getRounded(castIntToDouble(target1)/castIntToDouble(target2))
    else if (remainingObjects (head paths) border objects == []) && (takePathInArea (head paths) border)== (head paths) then helperforSuccess (tail paths) border objects (target1 + (length(head paths))) (target2+1)
    else  helperforSuccess (tail paths) border objects target1 target2
 


averageStepsInSuccess :: [[Point]] -> Dimensions -> [Point] -> Double
averageStepsInSuccess paths border objects = 
    
    helperforSuccess paths border objects 0 0
        







