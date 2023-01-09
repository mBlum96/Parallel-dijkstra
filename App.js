import React, { useState, useEffect } from "react";

async function getDistance(src, dest) {
    try {
        const response = await fetch(`http://localhost:12345/dijkstra?src=${src}&dest=${dest}`);
        const data = await response.json();
        return data.distance;
    } catch (error) {
        console.error(error);
        return null;
    }
}

function App() {
    const [src, setSrc] = useState(0);
    const [dest, setDest] = useState(0);
    const [distance, setDistance] = useState(null);
    useEffect(() => {
        getDistance(src, dest).then(distance => setDistance(distance));
    }, [src, dest]);
    return (
        <div>
            <label>
                Source vertex:
                <input type="number" value={src} onChange={event => setSrc(event.target.value)} />
            </label>
            <br />
            <label>
                Destination vertex:
                <input type="number" value={dest} onChange={event => setDest(event.target.value)} />
            </label>
            <br />
            {distance ? `Distance: ${distance}` : "Distance: N/A"}
        </div>
    );
}

export default App;
