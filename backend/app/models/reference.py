from sqlmodel import Field, Relationship, SQLModel
from typing import List

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from app.models.session import ACSession
    from app.models.derived import CornerPerformance, Event

class Track(SQLModel, table=True):
    __tablename__ = "tracks" # type: ignore

    track_id: int               = Field(primary_key=True)
    name: str | None            = Field(default=None, unique=True, index=True)
    layout_variant: str | None  = Field(default=None, index=True)

    corners: list["Corner"]         = Relationship(back_populates="track")
    ac_sessions: list["ACSession"]  = Relationship(back_populates="track")


class Car(SQLModel, table=True):
    __tablename__ = "cars" # type: ignore

    car_id: int                 = Field(primary_key=True)
    name: str | None            = Field(default=None, unique=True, index=True)
    max_rpm: int | None         = Field(default=None)
    tyre_radius: float | None   = Field(default=None)

    ac_sessions: list["ACSession"] = Relationship(back_populates="car")


class Corner(SQLModel, table=True):
    __tablename__ = "corners" # type: ignore

    corner_id: int              = Field(primary_key=True)
    corner_number: int | None   = Field(default=None)
    name: str | None            = Field(default=None, unique=True, index=True)

    track_id: int | None = Field(
        default=None,
        foreign_key="tracks.track_id"
    )

    track: "Track"                                  = Relationship(back_populates="corners")
    corner_performances: List["CornerPerformance"]  = Relationship(back_populates="corner")
    events: List["Event"]                           = Relationship(back_populates="corner")
